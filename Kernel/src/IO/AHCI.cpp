#include <AHCI.hpp>

namespace AHCI
{
	// Namespace Functions //

	forceinline byte GetPortType(HBAPort* port)
	{
		constexpr byte hbaDevicePresentAndConnected = 3;
		constexpr byte hbaIpmActive = 1;

		constexpr uint32 sataSigATAPI = 0xEB140101;
		constexpr uint32 sataSigATA = 0x101;
		constexpr uint32 sataSigSEMB = 0xC33C0101;
		constexpr uint32 sataSigPM = 0x9669101;

		fast u32 status = port->sataStatus;
		fast byte powman = (status >> 8) & 0b111;
		fast byte devdet = status & 0b111;

		if (devdet != hbaDevicePresentAndConnected) return PortType::None;
		if (powman != hbaIpmActive) return PortType::None;

		switch (port->Signature)
		{
		case sataSigATAPI:
			return PortType::SATAPI;

		case sataSigATA:
			return PortType::SATA;

		case sataSigSEMB:
			return PortType::SEMB;

		case sataSigPM:
			return PortType::PM;
		}

		return PortType::None;
	}

	// ATAPort //

	void ATAPort::Configure()
	{
		StopCMD();
		mMutex.Lock();

		fast vptr newBase = PageFrameAllocator::RequestPage();
		memset64(newBase, 0x00, PAGE_SIZE);
		hbaPort->CommandListBase = u32(newBase);
		hbaPort->CommandListBaseUpper = u32(u64(newBase) >> 32);

		fast vptr fisBase = PageFrameAllocator::RequestPage();
		memset64(fisBase, 0x00, PAGE_SIZE);
		hbaPort->fisBaseAddress = u32(fisBase);
		hbaPort->fisBaseAddressUpper = u32(u64(fisBase) >> 32);

		auto cmdHeader = (HBACommandHeader*)(u64(hbaPort->CommandListBase) | (u64(hbaPort->CommandListBaseUpper) << 32));

		for (u32 x = 0; x < 32; x++)
		{
			vptr cmdTableAddress = PageFrameAllocator::RequestPage();
			memset64(cmdTableAddress, 0x00, PAGE_SIZE);
			u64 address = u64(cmdTableAddress) + (x << 8);

			fast auto cmd = cmdHeader + x;
			{
				cmd->prdtLength = 8;
				cmd->CommandTableBase = address;
				cmd->CommandTableBaseUpper = address >> 32;
			}
		}

		mMutex.Unlock();
		StartCMD();
	}

	bool ATAPort::StartCMD()
	{
		mMutex.Lock();
		u64 timeout = 0;
		while (hbaPort->cmdStatus & HbaPxCMD_CR)
		{
			Yield();
			if (timeout++ > 100000)
			{
				mMutex.Unlock();
				return false;
			}
		}

		hbaPort->cmdStatus |= HbaPxCMD_FRE;
		hbaPort->cmdStatus |= HbaPxCMD_ST;
		mMutex.Unlock();
		return true;
	}

	bool ATAPort::StopCMD()
	{
		mMutex.Lock();

		hbaPort->cmdStatus &= (~HbaPxCMD_FRE);
		hbaPort->cmdStatus &= (~HbaPxCMD_ST);

		u64 timeout = 0;
		while (true)
		{
			if (timeout >= 100000)
			{
				mMutex.Unlock();
				return false;
			}
			if (hbaPort->cmdStatus & (HbaPxCMD_FR | HbaPxCMD_CR))
			{
				Yield();
				timeout++;
				continue;
			}
			break;
		}
		mMutex.Unlock();
		return true;
	}

	bool ATAPort::Read(uint64 sector, uint16 sectorCount, vptr buffer)
	{
		if (sectorCount == 0) return false;

		mMutex.Lock(); // Lock the port

		// the number of sectors the buffer can hold
		u32 bufferSectorCount = mBufferSize / SECTOR_SIZE;

		if (bufferSectorCount == 0)
		{
			mMutex.Unlock();
			return false;
		}
		// how many times the read buffer can be fully filled
		u32 fillCount = sectorCount / bufferSectorCount;

		// Get remainder of sectors
		sectorCount -= bufferSectorCount * fillCount;

		// Some offset values
		uint64 sectorOffset = sector;
		byte* bufferOffset = (byte*)buffer;

		for (nint x = 0; x < fillCount; x++)
		{
			// Fill the read buffer
			if (!ReadFillBuffer(sectorOffset))
			{
				// Retry one more time
				if (!ReadFillBuffer(sectorOffset))
				{
					// Give up
					mMutex.Unlock();
					return false;
				}
			}

			// Copy over contents to the caller's buffer
			memcpy(bufferOffset, mBuffer, mBufferSize);

			// Increase offsets
			sectorOffset += bufferSectorCount;
			bufferOffset += mBufferSize;
		}

		// Read the remainder now

		if (sectorCount == 0)
		{
			mMutex.Unlock();
			return true;
		}

		u32 sectorL = sectorOffset, sectorH = sectorOffset >> 32;

		hbaPort->InterruptStatus = ~((u32)0);
		auto cmdHeader = (HBACommandHeader*)(u64(hbaPort->CommandListBase) | (u64(hbaPort->CommandListBaseUpper) << 32));
		{
			cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32);
			cmdHeader->Write = 0;
			cmdHeader->prdtLength = 1;
		}

		auto commandTable = (HBACommandTable*)(u64(cmdHeader->CommandTableBase) | (u64(cmdHeader->CommandTableBaseUpper) << 32));
		{
			memset64(commandTable, 0x00, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HbaPrdtEntry));
			commandTable->prdtEntry->DataBase = (u32)mBuffer;
			commandTable->prdtEntry->DataBaseUpper = u32(u64(mBuffer) >> 32);
			commandTable->prdtEntry->ByteCount = (sectorCount * SECTOR_SIZE) - 1;
			commandTable->prdtEntry->InterruptOnCompleteion = 1;
		}

		auto cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);
		{
			cmdFIS->fisType = FisType::RegH2D;
			cmdFIS->CommandControl = 1; // this is a command
			cmdFIS->Command = AtaCMD::ReadDmaEX;

			cmdFIS->lba0 = (byte)sectorL;
			cmdFIS->lba1 = (byte)(sectorL >> 8);
			cmdFIS->lba2 = (byte)(sectorL >> 16);

			cmdFIS->lba3 = (byte)sectorH;
			cmdFIS->lba4 = (byte)(sectorH >> 8);
			cmdFIS->lba5 = (byte)(sectorH >> 16);

			cmdFIS->DeviceRegister = 1 << 6; // LBA Mode

			cmdFIS->CountLow = (byte)sectorCount;
			cmdFIS->CountHigh = (byte)(sectorCount >> 8);
		}

		u64 spinLock = 0;
		{
			while (hbaPort->TaskFileData & (AtaDevBusy | AtaDevDRQ))
			{
				if (spinLock++ > 100000)
				{
					// Give up
					mMutex.Unlock();
					return false;
				}
				Yield();
			}
		}

		hbaPort->CommandIssue = 1;

		spinLock = 0;
		while (spinLock++ < 100000)
		{
			if (!hbaPort->CommandIssue)
			{
				// Copy the rest over
				memcpy(bufferOffset, mBuffer, sectorCount * 512);
				mMutex.Unlock();
				return true;
			}
			if (hbaPort->InterruptStatus & HbaPxIS_TFES)
			{
				// Give up
				mMutex.Unlock();
				return false;
			}
			Yield();
		}

		// Give up
		mMutex.Unlock();
		return false;
	}

	bool ATAPort::ReadBytes(uint64 sector, uint32 bytecount, vptr buffer)
	{
		if (bytecount == 0) return false;

		uint32 sectorCount = (bytecount / SECTOR_SIZE) + 1;

		mMutex.Lock(); // Lock the port

		// the number of sectors the buffer can hold
		u32 bufferSectorCount = mBufferSize / SECTOR_SIZE;

		if (bufferSectorCount == 0)
		{
			mMutex.Unlock();
			return false;
		}
		// how many times the read buffer can be fully filled
		u32 fillCount = sectorCount / bufferSectorCount;

		// Get remainder of sectors
		sectorCount -= bufferSectorCount * fillCount;
		bytecount -= (bufferSectorCount * fillCount) * SECTOR_SIZE;

		// Some offset values
		uint64 sectorOffset = sector;
		byte* bufferOffset = (byte*)buffer;

		for (nint x = 0; x < fillCount; x++)
		{
			// Fill the read buffer
			if (!ReadFillBuffer(sectorOffset))
			{
				// Retry one more time
				if (!ReadFillBuffer(sectorOffset))
				{
					// Give up
					mMutex.Unlock();
					return false;
				}
			}

			// Copy over contents to the caller's buffer
			memcpy(bufferOffset, mBuffer, mBufferSize);

			// Increase offsets
			sectorOffset += bufferSectorCount;
			bufferOffset += mBufferSize;
		}

		// Read the remainder now

		if (sectorCount == 0)
		{
			mMutex.Unlock();
			return true;
		}

		u32 sectorL = sectorOffset, sectorH = sectorOffset >> 32;

		hbaPort->InterruptStatus = ~((u32)0);
		auto cmdHeader = (HBACommandHeader*)(u64(hbaPort->CommandListBase) | (u64(hbaPort->CommandListBaseUpper) << 32));
		{
			cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32);
			cmdHeader->Write = 0;
			cmdHeader->prdtLength = 1;
		}

		auto commandTable = (HBACommandTable*)(u64(cmdHeader->CommandTableBase) | (u64(cmdHeader->CommandTableBaseUpper) << 32));
		{
			memset64(commandTable, 0x00, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HbaPrdtEntry));
			commandTable->prdtEntry->DataBase = (u32)mBuffer;
			commandTable->prdtEntry->DataBaseUpper = u32(u64(mBuffer) >> 32);
			commandTable->prdtEntry->ByteCount = (sectorCount * 512) - 1;
			commandTable->prdtEntry->InterruptOnCompleteion = 1;
		}

		auto cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);
		{
			cmdFIS->fisType = FisType::RegH2D;
			cmdFIS->CommandControl = 1; // this is a command
			cmdFIS->Command = AtaCMD::ReadDmaEX;

			cmdFIS->lba0 = (byte)sectorL;
			cmdFIS->lba1 = (byte)(sectorL >> 8);
			cmdFIS->lba2 = (byte)(sectorL >> 16);

			cmdFIS->lba3 = (byte)sectorH;
			cmdFIS->lba4 = (byte)(sectorH >> 8);
			cmdFIS->lba5 = (byte)(sectorH >> 16);

			cmdFIS->DeviceRegister = 1 << 6; // LBA Mode

			cmdFIS->CountLow = (byte)sectorCount;
			cmdFIS->CountHigh = (byte)(sectorCount >> 8);
		}

		u64 spinLock = 0;
		{
			while (hbaPort->TaskFileData & (AtaDevBusy | AtaDevDRQ))
			{
				if (spinLock++ > 100000)
				{
					// Give up
					mMutex.Unlock();
					return false;
				}
				Yield();
			}
		}

		hbaPort->CommandIssue = 1;

		spinLock = 0;
		while (spinLock++ < 100000)
		{
			if (!hbaPort->CommandIssue)
			{
				// Copy the rest over
				memcpy(bufferOffset, mBuffer, bytecount);
				mMutex.Unlock();
				return true;
			}
			if (hbaPort->InterruptStatus & HbaPxIS_TFES)
			{
				// Give up
				mMutex.Unlock();
				return false;
			}
			Yield();
		}

		// Give up
		mMutex.Unlock();
		return false;
		return false;
	}

	bool ATAPort::ReadFillBuffer(uint64 sector)
	{
		if (!mMutex.IsLocked()) return false; // caller does not own port
		u32 sectorL = sector, sectorH = sector >> 32;
		uint16 sectorCount = mBufferSize / 512;

		hbaPort->InterruptStatus = ~((u32)0);
		auto cmdHeader = (HBACommandHeader*)(u64(hbaPort->CommandListBase) | (u64(hbaPort->CommandListBaseUpper) << 32));
		{
			cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32);
			cmdHeader->Write = 0;
			cmdHeader->prdtLength = 1;
		}

		auto commandTable = (HBACommandTable*)(u64(cmdHeader->CommandTableBase) | (u64(cmdHeader->CommandTableBaseUpper) << 32));
		{
			memset64(commandTable, 0x00, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HbaPrdtEntry));
			commandTable->prdtEntry->DataBase = (u32)mBuffer;
			commandTable->prdtEntry->DataBaseUpper = u32(u64(mBuffer) >> 32);
			commandTable->prdtEntry->ByteCount = (mBufferSize)-1;
			commandTable->prdtEntry->InterruptOnCompleteion = 1;
		}

		auto cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);
		{
			cmdFIS->fisType = FisType::RegH2D;
			cmdFIS->CommandControl = 1; // this is a command
			cmdFIS->Command = AtaCMD::ReadDmaEX;

			cmdFIS->lba0 = (byte)sectorL;
			cmdFIS->lba1 = (byte)(sectorL >> 8);
			cmdFIS->lba2 = (byte)(sectorL >> 16);

			cmdFIS->lba3 = (byte)sectorH;
			cmdFIS->lba4 = (byte)(sectorH >> 8);
			cmdFIS->lba5 = (byte)(sectorH >> 16);

			cmdFIS->DeviceRegister = 1 << 6; // LBA Mode

			cmdFIS->CountLow = (byte)sectorCount;
			cmdFIS->CountHigh = (byte)(sectorCount >> 8);
		}

		u64 spinLock = 0;
		{
			while (hbaPort->TaskFileData & (AtaDevBusy | AtaDevDRQ))
			{
				if (spinLock++ > 100000)
				{
					return false;
				}
				Yield();
			}
		}

		hbaPort->CommandIssue = 1;

		spinLock = 0;
		while (spinLock++ < 100000)
		{
			if (!hbaPort->CommandIssue)
			{
				return true;
			}
			if (hbaPort->InterruptStatus & HbaPxIS_TFES)
			{
				return false;
			}
			Yield();
		}
		// cant get here lol
		return false;
	}

	ATAPort::~ATAPort()
	{
		PageFrameAllocator::FreePage(mBuffer);
	}

	// AHCIDriver //

	AHCIDriver::AHCIDriver(PCIDeviceHeader* device)
	{
		this->mPCIBaseAddress = device;
		this->mABAR = (HBAMemory*)nint(((PCIHeader0*)device)->BAR5);

		PageTableManager::MapMemory(mABAR, mABAR);
		PageFrameAllocator::LockPage(mABAR);

		mPortCount = 0x00;
		ProbePorts();

		for (byte x = 0; x < mPortCount; x++)
		{
			auto port = mPorts[x];
			port->Configure();
		}
	}

	AHCIDriver::~AHCIDriver()
	{
		for (byte x = 0; x < mPortCount; x++)
		{
			delete mPorts[x];
		}
	}

	bool AHCIDriver::Read(byte portIndex, uint64 sector, uint16 sectorCount, vptr buffer)
	{
		if (portIndex >= mPortCount) return false;

		auto port = mPorts[portIndex];
		return port->Read(sector, sectorCount, buffer);
	}

	ATAPort* AHCIDriver::GetPort(byte portNum)
	{
		if (portNum >= mPortCount) return nullptr;
		return mPorts[portNum];
	}

	void AHCIDriver::AddPort(byte portType, HBAPort* hbaport)
	{
		fast ATAPort* port = new ATAPort();
		{
			port->mBuffer = PageFrameAllocator::RequestPage();
			port->mBufferSize = PAGE_SIZE;
			port->hbaPort = hbaport;
			port->PortNumber = mPortCount;
			port->PortType = portType;
		}
		mPorts[mPortCount++] = port;
	}

	void AHCIDriver::ProbePorts()
	{
		fast u32 portsImpl = mABAR->PortsImplemented;

		for (fast u32 x = 0; x < 32; x++)
		{
			if (portsImpl & (1 << x))
			{
				fast auto portptr = mABAR->Ports + x;
				byte portType = GetPortType(portptr);

				switch (portType)
				{
				case PortType::SATA:
					debug("\tSATA Drive Found!");
					AddPort(PortType::SATA, portptr);
					break;

				case PortType::SATAPI:
					debug("\tSATAPI Drive Found!");
					AddPort(PortType::SATAPI, portptr);
					break;

				case PortType::SEMB: debug("\SEMB Drive Found!");
					break;

				case PortType::PM: debug("\tPM Drive Found!");
					break;

				default: debug("\tUnknown Drive Found");
					break;
				}
			}
		}
	}

}