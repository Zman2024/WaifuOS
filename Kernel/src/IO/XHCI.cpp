#include <XHCI.h>
#include <IOBus.hpp>
#include <Scheduling.h>

namespace USB
{
	List<XHCIDriver*> XHCIDrivers;

	XHCIDriver::XHCIDriver(PCIDeviceHeader* device)
	{
		static bool _init = true;
		if (_init)
		{
			XHCIDrivers = List<XHCIDriver*>();
		}

		PageTableManager::MapMemory(device, device);
		PageFrameAllocator::LockPage(device);
		xhciDeviceHeader = (PCIHeader0*)device;

		Registers.Capability = (CapabilityRegister*)(u64(xhciDeviceHeader->BAR0) | (u64(xhciDeviceHeader->BAR1) << 32));
		PageTableManager::MapMemory(Registers.Capability, Registers.Capability);
		PageFrameAllocator::LockPage(Registers.Capability);

		Registers.Operational = (OperationalRegister*)(u64(Registers.Capability) + Registers.Capability->Length);
		PageTableManager::MapMemory(Registers.Operational, Registers.Operational);
		PageFrameAllocator::LockPage(Registers.Operational);

		PageTableManager::MapMemory(Registers.Operational->PortRegisterSet, Registers.Operational->PortRegisterSet);
		PageFrameAllocator::LockPage(Registers.Operational->PortRegisterSet);

		XHCIDrivers.Add(this);
	}

	XHCIDriver::~XHCIDriver()
	{
		XHCIDrivers.Remove(this);
	}

	void OperationalRegister::Start()
	{
		if (USBCommand & CMD_HC_RUN) return; // already running
		USBCommand |= CMD_HC_RUN;
	}

	void OperationalRegister::Stop()
	{
		if (!(USBCommand & CMD_HC_RUN)) return; // already stopped
		USBCommand &= CMD_HC_RUN;
	}

	void OperationalRegister::Reset()
	{
		Stop();
		this->USBCommand |= CMD_HC_RESET;
		Start();
	}

}