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
		byte LatencyTime;
		byte HeaderType;
		byte BIST;
	};

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