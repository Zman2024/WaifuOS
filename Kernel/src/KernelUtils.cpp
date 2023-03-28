#include <KernelUtils.h>

namespace Kernel
{
	void InitializeKernel(const BootInfo& bootInfo)
	{
		cli;

		#ifdef QEMU_WINDOWS_BUILD
		warn("================ QEMU WINDOWS BUILD ================");
		#endif

		// Enable Features like AVX, OSXSAVE etc.
		EnableHardwareFeatures();

		// Do GDT stuff
		InitializeGDT();

		// Basic Interrupts / CPU exceptions
		InitializeExceptions();

		// Paging / Memory Mapping
		InitializePaging(bootInfo);

		// malloc / free enabled
		Memory::InitializeHeap(0x100000000000L, 0x10000); // ~4 GB

		// OH NONONONO I HAVE DEPENDENCY LOOPS AAAAAAAAAAAAAAAAAA HELP //

		// IRQs (PIC)
		InitializeEarlyIRQ();

		// Scheduling
		Scheduler::Start();

		sti;

		// ACPI / PCI Enumeration
		InitializeACPI(bootInfo);

		// PCI Initialization
		PCI::InitializePCIDevices();

		// IRQs / MP (APIC)
		InitializeSMP();
	}

	void InitializeEarlyIRQ()
	{
		PIC::Mask(PicMask::Keyboard | PicMask::PIT | PicMask::RTC, true);

		// init stuff that can fire IRQs
		PIT::Initialize();
		RTC::Initialize();
	}

	void InitializeSMP(bool forceUsePic)
	{
		asm("pushfq");
		{
			cli;
			debug("Initializing IRQs...");

			if (APIC::UsableAPIC & !forceUsePic)
			{
				debug("\tUsing APIC");
				
				PIC::Disable();

				APIC::Initialize(); // inits both lapic and ioapic

				APIC::SetEntry(Interrupts::Interrupt::PIT); // Programable Interrupt Timer
				APIC::SetEntry(Interrupts::Interrupt::RTC); // IRQ 8 (RTC)
				APIC::SetEntry(Interrupts::Interrupt::Keyboard); // adds IRQ 1 entry (Keyboard)

				APIC::Enable();
			}
			else
			{
				debug("\tUsing PIC");
			}
		}
		asm("popfq");
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
		else warn("MCFG NOT FOUND! CANNOT USE PCI DEVICES!");

	}

	void ShowLoadingImage(const BootInfo& info)
	{
		if (!info.LoadingImage)
		{
			error("ERROR LOADING BOOT IMAGE!");
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
		RegisterInterrupt((vptr)hNonMaskableInterrupt, Interrupt::NonMaskable);
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

		GlobalGDT.TaskSS.Access.Present = 1;
		GlobalGDT.TaskSS.Access.PrivilegeLevel = 0;
		GlobalGDT.TaskSS.Access.DescriptorType = 0;
		GlobalGDT.TaskSS.Access.SystemDescriptorType = 0x9;
		GlobalGDT.TaskSS.SetLimit(sizeof(TaskStateSegment) - 1); // this needs to change if i want to use an IOPB (sizeof(tss) + 32)
		GlobalGDT.TaskSS.SetBase(&gTSS);

		GDTDescriptor desc{};
		desc.Size = sizeof(GDT) - 1;
		desc.Offset = (u64)&GlobalGDT;
		LoadGDT(&desc);

		nint tssIndex = (u64)(u64(&GlobalGDT.TaskSS) - u64(&GlobalGDT));
		gTSS.IOMapBase = 0x10;
		LoadTSS(tssIndex);
	}

	void EnableHardwareFeatures()
	{
		/*
			OK, so qemu is really stupid on winglows and "doesn't support AVX"
			which doesn't make sense because qemu will still execute AVX instructions
			without raising any #UD or crashing. HOWEVER it WILL freeze if i try and
			use OSXSAVE instructions like xgetbv and xsetbv, and the cpuid for AVX support
			will be false. In other words, i cant write a proper implementation for
			AVX that works with both QEMU on windows and real hardware
		*/

		debug("Enabling CPU Features...");

		// Check Features //

		CPU::GetFeatures();
		#ifndef QEMU_WINDOWS_BUILD
		if (!CPU::Features::SSE4_2)
		{
			error("CPU DOES NOT SUPPORT SSE4_2! CANNOT RUN OS!");
			OS_HLT;
		}
		if (!CPU::Features::AVX)
		{
			error("CPU DOES NOT SUPPORT AVX! CANNOT RUN OS!");
			OS_HLT;
		}
		if (!CPU::Features::AVX2)
		{
			error("CPU DOES NOT SUPPORT AVX2! CANNOT RUN OS!");
			OS_HLT;
		}
		if (!CPU::Features::OSXSAVE) 
		{
			error("CPU DOES NOT SUPPORT OSXSAVE! CANNOT RUN OS!");
			OS_HLT;
		}
		#endif

		// Enable Features //

		nint cr0 = 0;
		{
			asm("mov %0, %%cr0" : "=r"(cr0));
			cr0 &= ~(ControlRegister::CR0::EM);
			cr0 |= ControlRegister::CR0::MP;
			asm("mov %%cr0, %0" : : "r"(cr0));
		}

		nint cr4 = 0;
		{
			asm("mov %0, %%cr4" : "=r"(cr4));
			cr4 |= ControlRegister::CR4::OSFXSR;
			cr4 |= ControlRegister::CR4::OSXMMEXCPT;
			asm("mov %%cr4, %0" : : "r"(cr4));

			#ifndef QEMU_WINDOWS_BUILD
			
			CPU::GetFeatures();

			if (CPU::Features::AVX)
			{
				cr4 |= ControlRegister::CR4::OSXSAVE;
				//asm("mov %%cr4, %0" : : "r"(cr4));

				uint32 xcr0 = CPU::GetXCR0();
				xcr0 |= ControlRegister::XCR0::X87;
				xcr0 |= ControlRegister::XCR0::AVX;
				xcr0 |= ControlRegister::XCR0::SSE;
				CPU::SetXCR0(xcr0);
			}
			#endif
		}

		CPU::GetFeatures();

		if (!CPU::Features::AES)
			warn("\tCPU Does not support AES!");

		if (!CPU::Features::VAES)
			warn("\tCPU Does not support VAES!");

		if (!CPU::Features::RDRAND)
			warn("\tCPU Does not support RDRAND!");

		if (!CPU::Features::RDSEED)
			warn("\tCPU Does not support RDSEED!");

		if (!CPU::Features::SHA)
			warn("\tCPU Does not support SHA!");



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
		for (u64 x = PAGE_SIZE << 4; x < memorySize; x += PAGE_SIZE)
		{
			PageTableManager::MapMemory(x, x);
		}

		// Remap Framebuffer
		for (u64 x = fbBase; x < fbBase + fbSize; x += PAGE_SIZE)
		{
			PageTableManager::MapMemory(x, x);
			PageTableManager::SetVirtualFlag(x, PTFlag::Present, true);
			PageTableManager::SetVirtualFlag(x, PTFlag::ReadWrite, false);
		}
		
		// Enable paging (juuuust to be sure)
		{
			nint cr0 = 0;
			asm("mov %0, %%cr0" : "=r"(cr0));
			cr0 |= ControlRegister::CR0::PG;
			asm("mov %%cr0, %0" : : "r" (cr0));
		}

		// set nullptr and ~nullptr to an impossible address
		PageTableManager::MapMemory(nullptr, ~nint(nullptr));
		PageTableManager::SetVirtualFlag(nullptr, PTFlag::NX, true);
		PageTableManager::SetVirtualFlag(nullptr, PTFlag::UserSuper, true);
		PageTableManager::SetVirtualFlag(nullptr, PTFlag::ReadWrite, false);

		PageTableManager::MapMemory(~nint(nullptr), ~nint(nullptr));
		PageTableManager::SetVirtualFlag(~nint(nullptr), PTFlag::NX, true);
		PageTableManager::SetVirtualFlag(~nint(nullptr), PTFlag::UserSuper, true);
		PageTableManager::SetVirtualFlag(~nint(nullptr), PTFlag::ReadWrite, false);

		// Load PML4 into control3
		asm("mov %%cr3, %0" : : "r" (PML4));
	}

}