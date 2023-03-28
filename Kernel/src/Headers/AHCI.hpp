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

	struct ATAPort
	{
		HBAPort* hbaPort;
		byte PortType;
		byte* mBuffer;
		uint32 mBufferSize;
		byte PortNumber;

		void Configure();

		bool StartCMD();
		bool StopCMD();

		bool Read(uint64 sector, uint16 sectorCount, vptr buffer);
		bool ReadBytes(uint64 sector, uint32 bytecount, vptr buffer);

		~ATAPort();

	private:
		Mutex mMutex = Mutex();
		bool mReadOnlyMode = false;

		bool ReadFillBuffer(uint64 sector);

	};

	struct AHCIDriver
	{
		AHCIDriver(PCIDeviceHeader* device);

		~AHCIDriver();

		bool Read(byte portIndex, uint64 sector, uint16 sectorCount, vptr buffer);
		ATAPort* GetPort(byte portNum);
		forceinline nint GetPortCount() { return mPortCount; }

	private:

		void AddPort(byte portType, HBAPort* hbaport);
		void ProbePorts();

		PCIDeviceHeader* mPCIBaseAddress;
		HBAMemory* mABAR;
		ATAPort* mPorts[32];
		byte mPortCount;
	};
}

#endif