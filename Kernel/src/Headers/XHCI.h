#pragma once
#ifndef H_XHCI
#define H_XHCI
#include <Globals.h>
#include <PCI.h>
#include <PageMapIndexer.h>
#include <PageFrameAllocator.h>
#include <MMIO.hpp>

namespace USB
{
	using namespace PCI;
	using namespace MMIO;

	// Section 5.3 HC Capability Registers
	// https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/extensible-host-controler-interface-usb-xhci.pdf

	struct CapabilityRegister
	{
		byte Length;
		byte rsv0;
		Register16 InterfaceVersion;

		Register32 StructParams1;
		Register32 StructParams2;
		Register32 StructParams3;
		Register32 CapabilityParams1;

		Register32 DoorbellOffset;
		Register32 RuntimeRegisterSpceOffset;
		Register32 CapabilityParams2;
		byte rsv1[0]; // size is Length - 0x20
	};

	struct Port
	{
		Register32 StsCtrl;
		Register32 PowerStsCtrl;
		Register32 LinkInfo;
		Register32 HardwareLPMControl;
	};

	struct OperationalRegister
	{
		Register32 USBCommand;
		Register32 USBStatus;
		Register32 PageSize;

		byte rsv0[8];

		Register32 NotificationControl;
		Register32 CommandRingControl;
		byte rsv1[0x10];

		uint64 DeviceContextBaseAddress;
		Register32 Config;

		byte rsv2[0x3C4];

		Port PortRegisterSet[0x1000 / sizeof(Port)];

	};

	struct USBRegisters
	{
		CapabilityRegister* Capability;
		OperationalRegister* Operational;
	};

	struct XHCIDriver
	{
		XHCIDriver(PCIDeviceHeader* device);

		~XHCIDriver();

	private:
		PCIHeader0* xhciDeviceHeader;
		USBRegisters Registers;
	};
}
#endif