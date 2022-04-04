#include <PCI.h>

namespace PCI
{
	using namespace ACPI;

	void EnumerateFunction(u64 deviceAddress, u64 func)
	{
		u64 offset = func << 12;
		u64 funcAddress = deviceAddress + offset;

		PageTableManager::MapMemory(funcAddress, funcAddress);
		PCIDeviceHeader* pci = (PCIDeviceHeader*)funcAddress;
		if (!pci->DeviceID || pci->DeviceID == 0xFFFF) return;

		// Do stuff like cache or something idk

		debug("\tVendor: %x0, Device: %x1", pci->VendorID, pci->DeviceID);

	}

	void EnumerateDevice(u64 busAddress, u64 dev)
	{
		u64 offset = dev << 15;
		u64 deviceAddress = busAddress + offset;

		PageTableManager::MapMemory(deviceAddress, deviceAddress);
		PCIDeviceHeader* pci = (PCIDeviceHeader*)deviceAddress;
		if (!pci->DeviceID || pci->DeviceID == 0xFFFF) return;

		for (u64 func = 0; func < 8; func++)
		{
			EnumerateFunction(deviceAddress, func);
		}
	}

	void EnumerateBus(u64 baseAddress, u64 bus)
	{
		u64 offset = bus << 20;
		u64 busAddress = baseAddress + offset;

		PageTableManager::MapMemory(busAddress, busAddress);
		PCIDeviceHeader* pci = (PCIDeviceHeader*)busAddress;
		if (!pci->DeviceID || pci->DeviceID == 0xFFFF) return;

		for (u64 dev = 0; dev < 32; dev++)
		{
			EnumerateDevice(busAddress, dev);
		}

	}

	void EnumeratePCI(MCFGHeader* mcfg)
	{
		debug("MCFG Table: ");
		u32 entries = (mcfg->Length - sizeof(MCFGHeader)) / sizeof(DeviceConfig);

		for (u32 x = 0; x < entries; x++)
		{
			DeviceConfig* cfg = (DeviceConfig*)((u64)mcfg + sizeof(MCFGHeader) + (sizeof(DeviceConfig) * x));

			for (u64 bus = cfg->StartBus; bus < cfg->EndBus; bus++)
			{
				EnumerateBus(cfg->BaseAddress, bus);
			}

		}

	}
}
