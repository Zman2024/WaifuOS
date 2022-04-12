#include <Interrupts.h>
#include <PIC.hpp>
#include <APIC.h>
#include <PIT.h>
#include <RTC.h>
#include <Speaker.h>
#include <Keyboard.h>

namespace Interrupts
{

	global RegisterState* GetRegisterDump();
	global nint GlobalHandlerStubTable[];
	global nint GlobalInterruptTable[];
	nint GlobalInterruptTable[0xFF];

	inline PrimitiveConsole& Console = gConsole;

	void RegisterInterruptStubs()
	{
		IDTDescEntry* intDescEntry = (IDTDescEntry*)(GlobalIDTR.Offset);
		for (int x = 0; x < 0xFF; x++)
		{
			intDescEntry[x].Selector = 0x08;
			intDescEntry[x].TypeAttribs = IdtType::InterruptGate;
			intDescEntry[x].SetOffset((nint)GlobalHandlerStubTable[x]);

			GlobalInterruptTable[x] = (nint)hStub;
		}
	}

	void RegisterInterrupt(vptr handlerAddress, u16 inter, IdtType interruptType)
	{
		IDTDescEntry* intDescEntry = (IDTDescEntry*)(GlobalIDTR.Offset + inter * sizeof(IDTDescEntry));
		intDescEntry->TypeAttribs = interruptType;
		GlobalInterruptTable[inter] = (nint)handlerAddress;
	}

	forceinline void PanicScreen()
	{
		gConsole.DisableCursor();
		gConsole.Clear(Color::DarkRed);
		gConsole.SetBackgroundColor(Color::DarkRed);
		gConsole.SetForegroundColor(Color32(0xFFDDDDDD));
	}

	forceinline void PrintRegisterDump()
	{
		auto regs = GetRegisterDump();

		printlnf("rax: %x0 \nrbx: %x1 \nrcx: %x2 \nrdx: %x3 \n", regs->rax, regs->rbx, regs->rcx, regs->rdx);

		printlnf("rsi: %x0 \nrdi: %x1 \n", regs->rsi, regs->rdi);

		printlnf("r8: %x0 \nr9: %x1 \nr10: %x2 \nr11: %x3 \nr12: %x4 \nr13: %x5 \nr14: %x6 \nr15: %x7 \n",
			regs->r8, regs->r9, regs->r10, regs->r11, regs->r12, regs->r13, regs->r14, regs->r15);

		printlnf("rflags: %x0", regs->rflags);
	}

	void hDivideByZeroFault()
	{
		PanicScreen();
		Console.WriteLine("ERROR: DIVIDE BY ZERO FAULT!");
		PrintRegisterDump();
	}

	void hDebug()
	{
		Console.WriteLine("Debug int called");
	}

	void hNonMaskableFault()
	{
		Console.WriteLine("NMI");
	}

	void hBreakpointFault()
	{
		PanicScreen();
		Console.WriteLine("Breakpoint");
		PrintRegisterDump();
		halt;
	}

	void hOverflowTrap()
	{
		PanicScreen();
		Console.WriteLine("Overflow trap");
		PrintRegisterDump();
		halt;
	}

	void hBoundRangeFault()
	{
		PanicScreen();
		Console.WriteLine("Bound Range");
		PrintRegisterDump();
		halt;
	}

	void hInvalidOpcodeFault()
	{
		PanicScreen();
		Console.Write("Invalid Opcode at address: ???");
		PrintRegisterDump();
		halt;
	}

	void hCoprocessorNAFault()
	{
		PanicScreen();
		Console.WriteLine("Coprocessor NA");
		PrintRegisterDump();
		halt;
	}

	void hDoubleFault(nint intr, nint code)
	{
		Console.Clear(Color::Red);
		Console.SetBackgroundColor(Color::Red);
		Console.SetForegroundColor(Color::White);
		Console.WriteLine("ERROR: DOUBLE FAULT!");
		PrintRegisterDump();
		halt;
	}

	void hCoprocessorSegmentOverrunFault()
	{
		PanicScreen();
		Console.WriteLine("Segment Overrun!");
		PrintRegisterDump();
		halt;
	}

	void hInvalidStateSegmentFault(nint intr, nint code)
	{
		PanicScreen();
		printlnf("Invalid State Segment!\nCode: %x0", code);
		PrintRegisterDump();
		halt;
	}

	void hSegmentMissingFault(nint intr, nint code)
	{
		PanicScreen();
		printlnf("Segment Missing!\nCode: %x0", code);
		PrintRegisterDump();
		halt;
	}

	void hStackFault(nint intr, nint code)
	{
		PanicScreen();
		printlnf("Stack Exception!\nCode: %x0", code);
		PrintRegisterDump();
		halt;
	}

	void hGeneralProtectionFault(nint intr, nint code)
	{
		PanicScreen();
		printlnf("ERROR: GENERAL PROTECTION FAULT!\nCode: %x0", code);
		PrintRegisterDump();
		halt;
	}

	void hPageFault(nint intr, nint code)
	{
		PanicScreen();

		uint64 addr = 0;
		// cr2 contains the page fault linear address (PFLA)
		asm ("mov %0, %%cr2" : "=r" (addr) );

		printlnf("ERROR: PAGE FAULT!");
		printlnf("ATTEMPTED TO ACCESS ADDRESS: %x0", addr);

		printlnf("Error Code: %x0", code);

		PrintRegisterDump();
		halt;
	}

	void hCoprocessorFault()
	{
		PanicScreen();
		Console.WriteLine("ERROR: COPROCESSOR FAULT!\n");
		PrintRegisterDump();
		halt;
	}

	void hAlignmentCheck(nint intr, nint code)
	{
		PanicScreen();
		printlnf("Alignment Check \nCode: %x0\n", code);
		PrintRegisterDump();
		halt;
	}

	void hMachineCheck()
	{
		PanicScreen();
		Console.WriteLine("Machine Check\n");
		PrintRegisterDump();
		halt;
	}

	void hSIMDFault()
	{
		PanicScreen();
		Console.WriteLine("SIMD Exception\n");
		PrintRegisterDump();
		halt;
	}

	void hKeyboardInt()
	{
		IO::Keyboard::KeyEvent();

		// End of int
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);

	}

	void hPitTick()
	{
		PIT::Tick();
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);
	}

	void hRtcTick()
	{
		RTC::Tick();

		//RTC::EndOfInterrupt();
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);
	}

	void hStub(nint intr)
	{
		warn("[WARNING]: An interrupt without a handler was called! Interrupt: %x0", intr);
	}

}
