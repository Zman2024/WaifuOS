#pragma once
#ifndef H_AHCI
#define H_AHCI
#include <Globals.h>
#include <Memory.h>
#include <PCI.h>
#include <PageTableManager.h>
#include <PageFrameAllocator.h>

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
		uint32 CommandListBase;
		uint32 CommandListBaseUpper;

		uint32 fisBaseAddress;
		uint32 fisBaseAddressUpper;

		uint32 InterruptStatus;
		uint32 InterruptEnabled;

		uint32 cmdStatus;
		uint32 rsv0;

		uint32 TaskFileData;
		uint32 Signature;

		uint32 sataStatus;
		uint32 sataControl;
		uint32 sataError;
		uint32 sataActive;
		uint32 CommandIssue;
		uint32 sataNotification;

		uint32 fisSwitchControl;
		uint32 rsv1[11];

		uint32 Vendor[4];

	};

	struct HBAMemory
	{
		uint32 HostCapability;
		uint32 GlobalHostControl;
		uint32 InterruptStatus;
		uint32 PortsImplemented;

		uint32 Version;

		uint32 cccControl;
		uint32 cccPorts;
		uint32 EnclosureManagementLocation;
		uint32 EnclosureManagementControl;

		uint32 HostCapabilityExt;
		uint32 BIOSHandoffCtrlSts;

		uint8 rsv0[0x74];
		uint8 Vendor[0x60];

		HBAPort Ports[1];

	};

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
		byte* Buffer;
		byte PortNumber;

		inline void Configure()
		{
			StopCMD();

			fast vptr newBase = PageFrameAllocator::RequestPage();
			hbaPort->CommandListBase = u32(newBase);
			hbaPort->CommandListBaseUpper = u32(u64(newBase) >> 32);
			memset64(newBase, 0x00, PAGE_SIZE);

			fast vptr fisBase = PageFrameAllocator::RequestPage();
			hbaPort->fisBaseAddress = u32(fisBase);
			hbaPort->fisBaseAddressUpper = u32(u64(fisBase) >> 32);
			memset64(fisBase, 0x00, PAGE_SIZE);

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

			StartCMD();
		}

		inline bool StopCMD()
		{
			hbaPort->cmdStatus &= (~HbaPxCMD_FRE);
			hbaPort->cmdStatus &= (~HbaPxCMD_ST);

			u64 timeout = 0;
			while (true)
			{
				if (hbaPort->cmdStatus & (HbaPxCMD_FR | HbaPxCMD_CR))
				{
					pause;
					continue;
				}
				break;
			}
			return true;
		}

		inline bool StartCMD()
		{
			u64 timeout = 0;
			while (hbaPort->cmdStatus & HbaPxCMD_CR)
			{
				pause;
				if (timeout++ > 0x100000) return false;
			}
			
			hbaPort->cmdStatus |= HbaPxCMD_FRE;
			hbaPort->cmdStatus |= HbaPxCMD_ST;
			return true;
		}

		inline bool Read(uint64 sector, uint16 sectorCount, vptr buffer)
		{
			u32 sectorL = sector, sectorH = sector >> 32;
			
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
				commandTable->prdtEntry->DataBase = (u32)buffer;
				commandTable->prdtEntry->DataBaseUpper = u32(u64(buffer) >> 32);
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
					if (spinLock++ > 1000000) return false;
					pause;
				}
			}

			hbaPort->CommandIssue = 1;

			while (true)
			{
				if (!hbaPort->CommandIssue) return true;
				if (hbaPort->InterruptStatus & HbaPxIS_TFES) return false;
				pause;
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
				port->Buffer = PageFrameAllocator::RequestPage<byte>();
				memset64(port->Buffer, 0x00, PAGE_SIZE);
				port->Read(0, 4, port->Buffer);

				debug("\tDrive contents: " + string(port->Buffer));
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

						case PortType::SEMB: //debug("\SEMB Drive Found!");
							break;

						case PortType::PM: //debug("\tPM Drive Found!");
							break;

						default: //debug("\tUnknown Drive Found");
							break;
					}
				}
			}
		}

	private:

		forceinline void AddPort(byte portType, HBAPort* hbaport)
		{
			fast Port* port = new Port();
			{
				port->Buffer = nullptr;
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