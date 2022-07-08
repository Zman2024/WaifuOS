#include <XHCI.h>
#include <IOBus.hpp>

namespace USB
{
	XHCIDriver::XHCIDriver(PCIDeviceHeader* device)
	{
		PageTableManager::MapMemory(device, device);
		PageFrameAllocator::LockPage(device);
		xhciDeviceHeader = (PCIHeader0*)device;

		Registers.Capability = (CapabilityRegister*)(u64(xhciDeviceHeader->BAR0) | ((u64)xhciDeviceHeader->BAR1 << 32));
		PageTableManager::MapMemory(Registers.Capability, Registers.Capability);
		PageFrameAllocator::LockPage(Registers.Capability);

		Registers.Operational = (OperationalRegister*)(u64(Registers.Capability) + Registers.Capability->Length);
		PageTableManager::MapMemory(Registers.Operational, Registers.Operational);
		PageFrameAllocator::LockPage(Registers.Operational);
		PageTableManager::MapMemory(Registers.Operational->PortRegisterSet, Registers.Operational->PortRegisterSet);
		PageFrameAllocator::LockPage(Registers.Operational->PortRegisterSet);

		u32 timeout = 0;
		Registers.Operational->USBCommand |= 1;
		debug("\tUSBCommand: %x0", Registers.Operational->USBCommand);
		
		while (true)
		{
			if ((Registers.Operational->USBStatus & 0x1) == 0) break;
			if (timeout++ > 10000000)
			{
				error("\tusb didnt start up");
				debug("\tUSBCommand: %x0", Registers.Operational->USBCommand);
				debug("\tUSBStatus: %x0", Registers.Operational->USBStatus);
				return;
			}
		}

	}

	XHCIDriver::~XHCIDriver()
	{

	}

}