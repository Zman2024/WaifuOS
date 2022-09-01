#pragma once
#ifndef H_AHCI
#define H_AHCI
#include <Globals.h>
#include <Memory.h>
#include <PCI.h>
#include <PageTableManager.h>
#include <PageFrameAllocator.h>
#include <Synchronization.hpp>
#include <Scheduling.h>

namespace AHCI 
{
	using namespace PCI;

	namespace PortType
	{
		enum : byte
		{
			None = 0x00,
			SATA = 0x01,
			SEMB = 0x02,
			PM = 0x03,
			SATAPI = 0x04,
		};
	}

	namespace FisType
	{
		enum : byte
		{
			RegH2D = 0x27,
			RegD2H = 0x34,
			DmaACT = 0x39,
			DmaSetup = 0x41,
			Data = 0x46,
			Bist = 0x58,
			PioSetup = 0x5F,
			DevBits = 0xA1,
		};
	}

	namespace AtaCMD
	{
		enum : byte
		{
			ReadDmaEX = 0x25,
		};
	}

	struct HBAPort
	{
		volatile uint32 CommandListBase;
		volatile uint32 CommandListBaseUpper;

		volatile uint32 fisBaseAddress;
		volatile uint32 fisBaseAddressUpper;

		volatile uint32 InterruptStatus;
		volatile uint32 InterruptEnabled;

		volatile uint32 cmdStatus;
		volatile uint32 rsv0;

		volatile uint32 TaskFileData;
		volatile uint32 Signature;

		volatile uint32 sataStatus;
		volatile uint32 sataControl;
		volatile uint32 sataError;
		volatile uint32 sataActive;
		volatile uint32 CommandIssue;
		volatile uint32 sataNotification;

		volatile uint32 fisSwitchControl;
		volatile uint32 rsv1[11];

		volatile uint32 Vendor[4];

	} packed;

	struct HBAMemory
	{
		volatile uint32 HostCapability;
		volatile uint32 GlobalHostControl;
		volatile uint32 InterruptStatus;
		volatile uint32 PortsImplemented;

		volatile uint32 Version;

		volatile uint32 cccControl;
		volatile uint32 cccPorts;
		volatile uint32 EnclosureManagementLocation;
		volatile uint32 EnclosureManagementControl;

		volatile uint32 HostCapabilityExt;
		volatile uint32 BIOSHandoffCtrlSts;

		volatile uint8 rsv0[0x74];
		volatile uint8 Vendor[0x60];

		volatile HBAPort Ports[1];

	} packed;

	struct HBACommandHeader
	{
		uint8 CommandFISLength : 5;
		uint8 ATAPI : 1;
		uint8 Write : 1;
		uint8 Prefetchable : 1;

		uint8 Reset : 1;
		uint8 BIST : 1;
		uint8 ClearBusy : 1;
		uint8 rsv0 : 1;
		uint8 PortMultiplier : 4;

		uint16 prdtLength;
		uint32 prdtCount;

		uint32 CommandTableBase;
		uint32 CommandTableBaseUpper;

		uint32 rsv1[4];

	};

	struct FIS_REG_H2D
	{
		uint8 fisType;

		uint8 PortMultiplier : 4;
		uint8 rsv0 : 3;
		uint8 CommandControl : 1;

		uint8 Command;
		uint8 FeatureLow;

		uint8 lba0;
		uint8 lba1;
		uint8 lba2;
		uint8 DeviceRegister;

		uint8 lba3;
		uint8 lba4;
		uint8 lba5;
		uint8 FeatureHigh;

		uint8 CountLow;
		uint8 CountHigh;
		uint8 isoCommandCompletion;
		uint8 Control;

		uint8 rsv1[4];
	};

	struct FIS_REG_D2H
	{

	};

	struct HbaPrdtEntry
	{
		uint32 DataBase;
		uint32 DataBaseUpper;
		uint32 rsv0;

		uint32 ByteCount : 22;
		uint32 rsv1 : 9;
		uint32 InterruptOnCompleteion : 1;
	};

	struct HBACommandTable
	{
		uint8 CommandFIS[64];

		uint8 atapiCommand[16];
		uint8 rsv0[48];

		HbaPrdtEntry prdtEntry[];
	};

	constexpr uint32 HbaPxCMD_CR = 1 << 15;
	constexpr uint32 HbaPxCMD_FRE = 1 << 4;
	constexpr uint32 HbaPxCMD_ST = 1 << 0;
	constexpr uint32 HbaPxCMD_FR = 1 << 14;
	constexpr uint32 HbaPxIS_TFES = 1 << 30;

	constexpr uint32 SECTOR_SIZE = 0x200;

	constexpr uint32 AtaDevBusy = 0x80;
	constexpr uint32 AtaDevDRQ = 0x08;

	struct Port
	{
		HBAPort* hbaPort;
		byte PortType;
		byte* mBuffer;
		uint32 mBufferSize;
		byte PortNumber;

		inline void Configure()
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

		inline bool StopCMD()
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

		inline bool StartCMD()
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

		inline bool Read(uint64 sector, uint16 sectorCount, vptr buffer)
		{
			mMutex.Lock();

			// the number of sectors the buffer can hold
			u32 bufferSectorCount = mBufferSize / 512;

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
						memset64(buffer, 0x00, sectorCount * 512);
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

			if (sectorCount == 0) return true;

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
				commandTable->prdtEntry->ByteCount = (sectorCount * 512)-1;
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
						memset64(buffer, 0x00, sectorCount * 512);
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
					memset64(buffer, 0x00, sectorCount * 512);
					mMutex.Unlock();
					return false;
				}
				Yield();
			}

			// Give up
			memset64(buffer, 0x00, sectorCount * 512);
			mMutex.Unlock();
			return false;
		}

		inline ~Port()
		{
			PageFrameAllocator::FreePage(mBuffer);
		}

	private:
		
		Mutex mMutex = Mutex();

		inline bool ReadFillBuffer(uint64 sector)
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
				commandTable->prdtEntry->ByteCount = (mBufferSize) - 1;
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

	};

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

	struct AHCIDriver
	{
		inline AHCIDriver(PCIDeviceHeader* device)
		{
			this->mPCIBaseAddress = device;
			this->mABAR = (HBAMemory*)nint(((PCIHeader0*)device)->BAR5);

			PageTableManager::MapMemory(mABAR, mABAR);
			
			mPortCount = 0x00;
			ProbePorts();

			for (byte x = 0; x < mPortCount; x++)
			{
				auto port = mPorts[x]; 
				port->Configure();
			}
		}

		inline ~AHCIDriver()
		{
			for (byte x = 0; x < mPortCount; x++)
			{
				delete mPorts[x];
			}
		}

		inline void ProbePorts()
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

		inline bool Read(byte portIndex, uint64 sector, uint16 sectorCount, vptr buffer)
		{
			if (portIndex >= mPortCount) return false;

			auto port = mPorts[portIndex];
			return port->Read(sector, sectorCount, buffer);
		}

	private:

		forceinline void AddPort(byte portType, HBAPort* hbaport)
		{
			fast Port* port = new Port();
			{
				port->mBuffer = PageFrameAllocator::RequestPage();
				port->mBufferSize = PAGE_SIZE;
				port->hbaPort = hbaport;
				port->PortNumber = mPortCount;
				port->PortType = portType;
			}
			mPorts[mPortCount++] = port;
		}

		PCIDeviceHeader* mPCIBaseAddress;
		HBAMemory* mABAR;
		Port* mPorts[32];
		byte mPortCount;
	};
}

#endif