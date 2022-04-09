#pragma once
#ifndef H_PCI
#define H_PCI
#include <Types.h>
#include <ACPI.h>
#include <PageTableManager.h>
#include <cstr.h>
#include <Globals.h>

namespace PCI
{
	// TODO: add all the PCI headers https://wiki.osdev.org/PCI

	struct PCIDeviceHeader
	{
		uint16 VendorID;
		uint16 DeviceID;
		uint16 Command;
		uint16 Status;

		byte RevisionID;
		byte ProgIF;
		byte Subclass;
		byte Class;
		byte CacheLineSize;
		byte LatencyTimer;
		byte HeaderType; // bit 7 indicates if this device has multiple functions (MF bit)
		byte BIST;
	};

	// Standard header
	struct PCIHeader0 : public PCIDeviceHeader
	{
		uint32 BAR0;
		uint32 BAR1;
		uint32 BAR2;
		uint32 BAR3;
		uint32 BAR4;
		uint32 BAR5;

		uint32 CardbusCISPtr;
		uint16 SubsystemVendorID;
		uint16 SubsystemID;
		uint32 ExpansionROMBaseAddr;
		byte CapabilitiesPtr;

		byte Rsv0;
		uint16 Rsv1;
		uint32 Rsv2;

		byte InterruptLine;
		byte InterruptPin;

		byte MinGrant;
		byte MaxLatency;

	};

	// (PCI-to-PCI bridge)
	struct PCIHeader1 : public PCIDeviceHeader
	{
		uint32 BAR0;
		uint32 BAR1;
		
		byte PrimaryBus;
		byte SecondaryBus;
		byte SubordinateBus;
		byte SecondaryLatencyTimer;

		byte IOBase;
		byte IOLimit;
		uint16 SecondaryStatus;

		uint16 MemoryBase;
		uint16 MemoryLimit;

		uint16 PrefetchMemoryBase;
		uint16 PrefetchMemoryLimit;

		uint32 PrefetchMemoryBaseUpper;

		uint32 PrefetchMemoryLimitUpper;

		uint16 IOBaseUpper;
		uint16 IOLimitUpper;

		byte CapabilityPtr;
		byte rsv0[3];

		uint32 ExpROMBaseAddress;

		byte InterruptLine;
		byte InterruptPin;
		uint16 BridgeControl;
	};

	struct DeviceConfig
	{
		uint64 BaseAddress;
		uint16 PCISegGroup;
		byte StartBus;
		byte EndBus;
		uint32 Reserved;
	} attribute((packed));

	void EnumeratePCI(ACPI::MCFGHeader* mcfg);
}

#endif