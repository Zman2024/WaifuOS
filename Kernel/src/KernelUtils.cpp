#include <KernelUtils.h>

namespace Kernel
{
	void InitializeKernel(const BootInfo& bootInfo)
	{
		cli;

		// Do GDT stuff
		InitializeGDT();

		// Basic Interrupts / CPU exceptions
		InitializeExceptions();

		// Paging / Memory Mapping
		InitializePaging(bootInfo);

		// malloc / free enabled
		Memory::InitializeHeap(0x100000000000L, 0x1000);

		// ACPI / PCI
		InitializeACPI(bootInfo);

		// IRQs
		InitializeIRQ();

		sti;
	}

	void InitializeIRQ(bool forceUsePic)
	{
		debug("Initializing IRQs...");
		
		if (APIC::UsableAPIC & !forceUsePic)
		{
			debug("\tUsing APIC");

			APIC::Initialize(); // inits both lapic and ioapic

			APIC::SetEntry(Interrupts::Interrupt::PIT); // Programable Interrupt Timer
			APIC::SetEntry(Interrupts::Interrupt::RTC); // IRQ 8 (RTC)
			APIC::SetEntry(Interrupts::Interrupt::Keyboard); // adds IRQ 1 entry (Keyboard)

			APIC::Enable();
		}
		else
		{
			debug("\tUsing PIC");
			PIC::Mask(PicMask::Keyboard | PicMask::PIT | PicMask::RTC, true);
		}

		// init stuff that can fire IRQs
		PIT::Initialize();
		RTC::Initialize();

	}

	void InitializeACPI(const BootInfo& bootInfo)
	{
		using namespace ACPI;
		debug("Initializing ACPI...");

		XSDTHeader* xsdt = (XSDTHeader*)bootInfo.RSDP->XSDTAddress;
		if (!xsdt)
		{
			error("ROOT XSDT NOT FOUND! CANNOT INITIALIZE ACPI DEVICES!");
			return;
		}

		PrintTables(xsdt);

		APIC::MADTHeader* madt = (APIC::MADTHeader*)FindTable(xsdt, "APIC");
		if (madt) APIC::ParseMADT(madt);
		else warn("APIC NOT FOUND! USING FALLBACK PIC!");

		MCFGHeader* mcfg = (MCFGHeader*)FindTable(xsdt, "MCFG");
		if (mcfg) PCI::EnumeratePCI(mcfg);
		else warn("MCFG NOT FOUND!");

	}

	void ShowLoadingImage(const BootInfo& info)
	{
		if (!info.LoadingImage)
		{
			gConsole.WriteLine("ERROR LOADING BOOT IMAGE!", Color::Red);
			return;
		}

		constexpr uint16 ImageWidth = 256;
		constexpr uint16 ImageHeight = 256;

		fast Color32* colorFB = (Color32*)info.Framebuffer->BaseAddress;
		fast Color32* img = (Color32*)(info.LoadingImage);

		for (fast u16 y = 0; y < ImageHeight; y++)
		{
			for (fast u16 x = 0; x < ImageWidth; x++)
			{
				Color32 pixel = img[(y * (ImageWidth)) + x];
				byte alpha = (pixel & 0xFF000000) >> 24;
				if (!alpha) continue;

				// wow this is massive, cringe. 10/10 very readable code
				// fixed a bit, seperated things into variables, still looks cringe
				fast uint64 framebufferOffsetY = u64((y + (u64)(info.Framebuffer->Height / 1.5) - (ImageHeight / 2)) * info.Framebuffer->PixelsPerScanline);
				fast uint64 framebufferOffsetX = u64(x + (info.Framebuffer->Width / 2) - (ImageWidth / 2));
				colorFB[framebufferOffsetY + framebufferOffsetX] = pixel;
			}
		}

	}

	global nint GlobalInterruptTable[];

	void InitializeExceptions()
	{
		using namespace Interrupts;
		
		debug("Loading Interrupt Vector Table (IDTR)...");

		// Setup GIDTR
		GlobalIDTR.Limit = 0x0FFF;
		GlobalIDTR.Offset = (u64)GlobalIDTROffset;
		memset64((vptr)GlobalIDTR.Offset, 0x00, PAGE_SIZE);

		// Load global IDT
		LoadGIDT();

		debug("Registering Fault / Interrupt Handlers...");

		RegisterInterruptStubs();

		// Remap and mask all ints in PIC (make sure PIC is off for now)
		PIC::Disable();
		PIC::Remap();

		// Register exceptions
		RegisterInterrupt((vptr)hDivideByZeroFault, Interrupt::DivideByZero);
		RegisterInterrupt((vptr)hDebug, Interrupt::Debug);
		RegisterInterrupt((vptr)hNonMaskableFault, Interrupt::NonMaskable);
		RegisterInterrupt((vptr)hBreakpointFault, Interrupt::Breakpoint, IdtType::TrapGate);
		RegisterInterrupt((vptr)hOverflowTrap, Interrupt::OverflowTrap, IdtType::TrapGate);
		RegisterInterrupt((vptr)hBoundRangeFault, Interrupt::BoundRangeExceeded);
		RegisterInterrupt((vptr)hInvalidOpcodeFault, Interrupt::InvalideOpcode);
		RegisterInterrupt((vptr)hCoprocessorNAFault, Interrupt::CoprocessorNA);
		RegisterInterrupt((vptr)hDoubleFault, Interrupt::DoubleFault);
		RegisterInterrupt((vptr)hCoprocessorSegmentOverrunFault, Interrupt::CoprocessorSegmentOverrun);
		RegisterInterrupt((vptr)hInvalidStateSegmentFault, Interrupt::InvalidStateSegment);
		RegisterInterrupt((vptr)hSegmentMissingFault, Interrupt::SegmentMissing);
		RegisterInterrupt((vptr)hStackFault, Interrupt::StackException);
		RegisterInterrupt((vptr)hGeneralProtectionFault, Interrupt::GeneralProtection);
		RegisterInterrupt((vptr)hPageFault, Interrupt::PageFault);
		RegisterInterrupt((vptr)hCoprocessorFault, Interrupt::CoprocessorError);
		RegisterInterrupt((vptr)hAlignmentCheck, Interrupt::AlignmentCheck);
		RegisterInterrupt((vptr)hMachineCheck, Interrupt::MachineCheck);
		RegisterInterrupt((vptr)hSIMDFault, Interrupt::SIMDException);

		// Add IRQ handlers to IDT
		RegisterInterrupt((vptr)hKeyboardInt, Interrupt::Keyboard);
		RegisterInterrupt((vptr)hPitTick, Interrupt::PIT);
		RegisterInterrupt((vptr)hRtcTick, Interrupt::RTC);
	}

	void InitializeGDT()
	{
		debug("Loading Global Descriptor Table (GDT)...");
		// Create and load GDT
		GDTDescriptor desc{};
		desc.Size = sizeof(GDT) - 1;
		desc.Offset = (u64)&GlobalGDT;
		LoadGDT(&desc);
	}

	void InitializePaging(const BootInfo& bootInfo)
	{
		debug("Initializing Paging...");
		u64 nMapEntries = bootInfo.MapSize / bootInfo.MapDescriptorSize;

		// Read EFI memory map and set up PFA
		PageFrameAllocator::ReadEfiMemoryMap(bootInfo.MemoryMap, bootInfo.MapSize, bootInfo.MapDescriptorSize);

		// Lock the framebuffer
		u64 fbBase = (u64)bootInfo.Framebuffer->BaseAddress;
		u64 fbSize = (u64)bootInfo.Framebuffer->BufferSize + PAGE_SIZE;
		PageFrameAllocator::LockPages((void*)fbBase, fbSize / PAGE_SIZE);

		// Lock the kernel
		u64 kSize = (u64)&_KernelEnd - (u64)&_KernelStart;
		PageFrameAllocator::LockPages(&_KernelStart, kSize / PAGE_SIZE + 1);

		// Initialize PageTableManager
		PageTable* PML4 = PageFrameAllocator::RequestPage<PageTable>();
		memset64(PML4, 0x00, PAGE_SIZE);
		PageTableManager::PageTableManager(PML4);

		// Map all memory (this causes rainbow in qemu)
		u64 memorySize = Memory::CalculateMemorySize(bootInfo.MemoryMap, nMapEntries, bootInfo.MapDescriptorSize);
		for (u64 x = 0; x < memorySize; x += PAGE_SIZE)
		{
			PageTableManager::MapMemory(x, x);
		}

		// Remap Framebuffer
		for (u64 x = fbBase; x < fbBase + fbSize; x += PAGE_SIZE)
		{
			PageTableManager::MapMemory(x, x);
		}
		
		// Enable paging (juuuust to be sure)
		{
			nint cr0 = 0;
			asm("mov %0, %%cr0" : "=r"(cr0));
			cr0 |= 0x80000000;
			asm("mov %%cr0, %0" : : "r" (cr0));
		}

		// set nullptr RW, US and present flags
		PageTableManager::SetVirtualFlag(nullptr, PTFlag::ReadWrite, false);
		PageTableManager::SetVirtualFlag(nullptr, PTFlag::UserSuper, false);
		PageTableManager::SetVirtualFlag(nullptr, PTFlag::Present, true);

		// Load PML4 into control3
		asm("mov %%cr3, %0" : : "r" (PML4));
	}

}