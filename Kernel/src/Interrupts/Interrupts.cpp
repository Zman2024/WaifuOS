#include <Interrupts.h>
#include <PIC.hpp>
#include <APIC.h>
#include <PIT.h>
#include <RTC.h>
#include <Speaker.h>
#include <Keyboard.h>
#include <Scheduling.h>

namespace Interrupts
{

	global nint GlobalHandlerStubTable[];
	global nint GlobalInterruptTable[];
	nint GlobalInterruptTable[0xFF];

	inline PrimitiveConsole& Console = gConsole;

	void RegisterInterruptStubs()
	{
		IDTDescEntry* intDescEntry = (IDTDescEntry*)(GlobalIDTR.Offset);
		for (int x = 0; x < 0xFF; x++)
		{
			intDescEntry[x].Selector = (SegmentSelector)KERNEL_CODE_GDT_INDEX << 3;
			intDescEntry[x].TypeAttribs = IdtType::InterruptGate;
			intDescEntry[x].SetOffset((nint)GlobalHandlerStubTable[x]);
			intDescEntry[x].IST = 0x00;
			GlobalInterruptTable[x] = (nint)hStub;
		}
	}

	void RegisterInterrupt(vptr handlerAddress, u16 inter, IdtType interruptType, byte ist, SegmentSelector selector)
	{
		IDTDescEntry* intDescEntry = (IDTDescEntry*)(GlobalIDTR.Offset + inter * sizeof(IDTDescEntry));
		intDescEntry->TypeAttribs = interruptType;
		intDescEntry->Selector = selector;
		intDescEntry->IST = ist;

		GlobalInterruptTable[inter] = (nint)handlerAddress;
	}

	void UnregisterInterrupt(u16 inter)
	{
		IDTDescEntry* intDescBase = (IDTDescEntry*)(GlobalIDTR.Offset);
		intDescBase[inter].TypeAttribs = IdtType::InterruptGate;
		intDescBase[inter].IST = 0x00;
		intDescBase[inter].Selector = (SegmentSelector)KERNEL_CODE_GDT_INDEX << 3;

		GlobalInterruptTable[inter] = (nint)hStub;
	}

	forceinline void PanicScreen()
	{
		gConsole.DisableCursor();
		gConsole.Clear(Color::DarkRed);
		gConsole.SetBackgroundColor(Color::DarkRed);
		gConsole.SetForegroundColor(Color32(0xFFDDDDDD));
	}

	forceinline void PrintRegisterDump(InterruptFrame* frame)
	{
		auto regs = GetRegisterDump();

		println("rax: %x0\trbx: %x1 \nrcx: %x2\trdx: %x3 \n", regs->rax, regs->rbx, regs->rcx, regs->rdx);

		println("rsi: %x0\trdi: %x1 \n", regs->rsi, regs->rdi);

		println("rsp: %x0\trbp: %x1 \n", frame->rsp, regs->rbp);

		println("r8: %x0\tr9: %x1\tr10: %x2 \nr11: %x3\tr12: %x4\tr13: %x5 \n\tr14: %x6\tr15: %x7 \n",
				 regs->r8, regs->r9, regs->r10, regs->r11, regs->r12, regs->r13, regs->r14, regs->r15);

		println("rip: %x0 \n", frame->rip);

		println("cs: %x0\tss: %x1 \nds: %x2\tes: %x3", frame->cs, frame->ss, regs->ds, regs->es);
		println("gs: %x0\tfs: %x1 \n", regs->gs, regs->fs);

		println("rflags: %x0 \n", frame->rflags);

		println("Threads: %0 \nThreads Since Boot: %1", Scheduler::GetThreadCount(), Scheduler::GetThreadsCreated());
		auto thread = Scheduler::GetCurrentThread();
		if (thread)
		{
			print("Current Thread: "); println(thread->Name);
		}
	}

	void hDivideByZeroFault(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("ERROR: DIVIDE BY ZERO FAULT!");
		PrintRegisterDump(frame);
	}

	void hDebug()
	{
		Console.WriteLine("Debug int called");
	}

	void hNonMaskableInterrupt(nint code, InterruptFrame* frame)
	{
		warn("NMI");
	}

	void hBreakpointFault(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("Breakpoint");
		PrintRegisterDump(frame);
		halt;
	}

	void hOverflowTrap(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("Overflow trap");
		PrintRegisterDump(frame);
		halt;
	}

	void hBoundRangeFault(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("Bound Range");
		PrintRegisterDump(frame);
		halt;
	}

	void hInvalidOpcodeFault(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		println("Invalid Opcode at address: %x0", frame->rip);
		PrintRegisterDump(frame);
		halt;
	}

	void hCoprocessorNAFault(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("Coprocessor NA");
		PrintRegisterDump(frame);
		halt;
	}

	void hDoubleFault(nint intr, InterruptFrame* frame, nint code)
	{
		gConsole.DisableCursor();
		Console.Clear(Color::Red);
		Console.SetBackgroundColor(Color::Red);
		Console.SetForegroundColor(Color::White);
		println("ERROR: DOUBLE FAULT! \nCODE: %x0", code);
		PrintRegisterDump(frame);
		halt;
	}

	void hCoprocessorSegmentOverrunFault(nint code, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("Segment Overrun!");
		PrintRegisterDump(frame);
		halt;
	}

	void hInvalidStateSegmentFault(nint intr, InterruptFrame* frame, nint code)
	{
		PanicScreen();
		println("Invalid Task State Segment!\nCode: %x0", code);
		PrintRegisterDump(frame);
		halt;
	}

	void hSegmentMissingFault(nint intr, InterruptFrame* frame, nint code)
	{
		PanicScreen();
		println("Segment Missing!\nCode: %x0", code);
		PrintRegisterDump(frame);
		halt;
	}

	void hStackFault(nint intr, InterruptFrame* frame, nint code)
	{
		PanicScreen();
		println("Stack-Segment Fault!\nCode: %x0", code);
		PrintRegisterDump(frame);
		halt;
	}

	void hGeneralProtectionFault(nint intr, InterruptFrame* frame, nint code)
	{
		PanicScreen();
		println("ERROR: GENERAL PROTECTION FAULT!\nCode: %x0", code);
		PrintRegisterDump(frame);
		halt;
	}

	void hPageFault(nint intr, InterruptFrame* frame, nint code)
	{
		PanicScreen();

		char* bitDefs[] =
		{
			"    Present Flag: %0",
			"    Read/Write Flag: %0\n",
			"    User/Supervisor Flag: %0",
			"    Reserved Flag: %0\n",
			"    Instruction/Data Flag: %0",
			"    Protection-Key Violation: %0\n",
			"    Shadow-Stack Access Fault: %0"
		};

		uint64 addr = 0;
		// cr2 contains the page fault linear address (PFLA)
		asm("mov %0, %%cr2" : "=r" (addr));

		println("ERROR: PAGE FAULT!");
		println("ATTEMPTED TO ACCESS ADDRESS: %x0", addr);

		println("Error Code: %x0", code);

		nint codeTemp = code;
		for (byte x = 0; x < 7; x++)
		{
			print(bitDefs[x], codeTemp & 1);
			codeTemp >>= 1;
		}

		println("    SGX Violaton: %0\n", code & (1 << 15));

		PrintRegisterDump(frame);

		halt;
	}

	void hCoprocessorFault(nint intr, InterruptFrame* frame)
	{
		PanicScreen();
		println("ERROR: COPROCESSOR FAULT!\n");
		PrintRegisterDump(frame);
		halt;
	}

	void hAlignmentCheck(nint intr, InterruptFrame* frame, nint code)
	{
		PanicScreen();
		println("Alignment Check \nCode: %x0\n", code);
		PrintRegisterDump(frame);
		halt;
	}

	void hMachineCheck(nint intr, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("Machine Check\n");
		PrintRegisterDump(frame);
		halt;
	}

	void hSIMDFault(nint intr, InterruptFrame* frame)
	{
		PanicScreen();
		Console.WriteLine("SIMD Exception\n");
		PrintRegisterDump(frame);
		halt;
	}

	void hKeyboardInt()
	{
		IO::Keyboard::KeyEvent();

		// End of int
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);

	}

	void hPitTick(nint intr)
	{
		PIT::Tick();
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);
	}

	void hRtcTick()
	{
		RTC::Tick();
		RTC::EndOfInterrupt();
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);
	}

	void hPitScheduler(nint intr, InterruptFrame* frame)
	{
		Scheduler::TimerInterrupt(GetRegisterDump(), frame);

		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO((intr - IRQ_OFFSET) > 7);
	}

	void hStub(nint intr)
	{
		warn("[WARNING]: An interrupt without a handler was called! Interrupt: %x0", intr);
	}

}
