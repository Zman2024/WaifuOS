#include <PCI.h>
#include <AHCI.hpp>
#include <XHCI.h>
#include <List.h>
#include <FAT32.h>

namespace PCI
{
	using namespace ACPI;

	List<PCIDeviceHeader*> SupportedFunctions;

	void EnumerateFunction(u64 deviceAddress, u64 func)
	{
		u64 offset = func << 12;
		u64 funcAddress = deviceAddress + offset;

		PageTableManager::MapMemory(funcAddress, funcAddress);
		PageFrameAllocator::LockPage((vptr)funcAddress);
		PCIDeviceHeader* pci = (PCIDeviceHeader*)funcAddress;
		if (!pci->DeviceID || pci->DeviceID == 0xFFFF) return;

		byte lclass = pci->Class, subclass = pci->Subclass, progif = pci->ProgIF;

		switch (lclass)
		{
			case 0x01: // Mass Storage Controller
				switch (subclass)
				{
					case 0x06: // SATA
						switch (progif)
						{
							case 0x01: // ACHI 1.0 Host
								debug("\tFound ACHI 1.0 Device");
								SupportedFunctions.Add(pci);
								// new AHCI::AHCIDriver(pci);
								break;
						}
						break;
				}
				break;

			case 0x08: // Base System Peripheral
				switch (subclass)
				{
					case 0x02: // Timer
						break;

					case 0x03: // RTC Controller
						switch (progif)
						{
							case 0x00: // Generic RTC
								debug("\tFound Generic RTC");
								break;

							case 0x01: // ISA-Compatible
								debug("\tFound ISA-Compatible RTC");
								break;
						}
						break;
				}
				break;

			case 0x09: // Input Device Controller
				switch (subclass)
				{
					case 0x00: // Keyboard Controller
						debug("\tFound Keyboard Controller");
						break;

					case 0x02: // Mouse Controller
						debug("\tFound Mouse Controller");
						break;
				}
				break;

			case 0x0C: // Serial Bus Controller
				switch (subclass)
				{
					case 0x03: // USB Controller
						switch (progif)
						{
							case 0x00: // UHCI Controller
								debug("\tFound USB UHCI Controller");
								break;

							case 0x10: // OHCI Controller
								debug("\tFound USB OHCI Controller");
								break;

							case 0x20: // EHCI (USB2) Controller
								debug("\tFound USB ECHI Controller (USB2)");
								break;

							case 0x30: // XHCI (USB3) Controller
								debug("\tFound USB XHCI Controller (USB3)");
								SupportedFunctions.Add(pci);
								break;

							case 0x80: // Unspecified
								debug("\tFound USB Controller (Unspecified)");
								break;

							case 0xFE: // USB Device (Not a host controller)
								debug("\tFound USB Device");
								break;

						}
						break;
				}
				break;

			case 0x0D: // Wireless Controller
				switch (subclass)
				{
					case 0x11: // Bluetooth Controller
						debug("\tFound Bluetooth Controller");
						break;

					case 0x12: // Broadband Controller
						debug("\tFound Broadband Controller");
						break;

					case 0x20: // Ethernet Controller (802.1a)
						debug("\tFound Ethernet Controller (802.1a)");
						break;

					case 0x21: // Ethernet Controller (802.1b)
						debug("\tFound Ethernet Controller (802.1b)");
						break;
				}
				break;

			default: 
				// debug("\tVendor: %x0, Device: %x1", pci->VendorID, pci->DeviceID);
				break;
		}

	}

	void EnumerateDevice(u64 busAddress, u64 dev)
	{
		u64 offset = dev << 15;
		u64 deviceAddress = busAddress + offset;

		PageTableManager::MapMemory(deviceAddress, deviceAddress);
		PageFrameAllocator::LockPage((vptr)deviceAddress);
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
		PageFrameAllocator::LockPage((vptr)busAddress);
		PCIDeviceHeader* pci = (PCIDeviceHeader*)busAddress;
		if (!pci->DeviceID || pci->DeviceID == 0xFFFF) return;

		for (u64 dev = 0; dev < 32; dev++)
		{
			EnumerateDevice(busAddress, dev);
		}

	}

	void EnumeratePCI(MCFGHeader* mcfg)
	{
		static bool _init = true;
		if (_init)
		{
			SupportedFunctions = List<PCIDeviceHeader*>();
			_init = false;
		}

		debug("MCFG Table: ");
		u32 entries = (mcfg->Length - sizeof(MCFGHeader)) / sizeof(DeviceConfig);

		for (u32 x = 0; x < entries; x++)
		{
			DeviceConfig* cfg = (DeviceConfig*)((u64)mcfg + sizeof(MCFGHeader) + (sizeof(DeviceConfig) * x));

			for (nint bus = cfg->StartBus; bus < cfg->EndBus; bus++)
			{
				EnumerateBus(cfg->BaseAddress, bus);
			}

		}

	}

	void InitializePCIDevices()
	{
		constexpr uint32 SIG_AHCI = 0x010601;
		constexpr uint32 SIG_XHCI = 0x0C0330;

		debug("Initializing PCI...");

		for (nint x = 0; x < SupportedFunctions.GetCount(); x++)
		{
			PCIDeviceHeader* pci = SupportedFunctions.Get(x);
			uint32 sig = (nint(pci->Class) << 16) | (nint(pci->Subclass) << 8) | (nint(pci->ProgIF) << 0);
			switch (sig)
			{
				case SIG_AHCI:
				{
					debug("\tInitializing ACHI 1.0 Driver");
					auto ahci = new AHCI::AHCIDriver(pci);
					auto port = ahci->GetPort(0);
					if (port && FAT32::DoesDriveContainF32(port))
					{
						// Has fat32 partition at mbr
						auto fs = new FAT32::FSDriver(port);

					}
					
					break;
				}

				case SIG_XHCI:
					debug("\tInitializing XHCI Driver");
					new USB::XHCIDriver(pci);
					break;
			}
		}
	}

}
