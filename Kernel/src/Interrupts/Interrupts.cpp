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
			intDescEntry[x].SetOffset((nint)GlobalHandlerStubTable[x]);

			GlobalInterruptTable[x] = (nint)hStub;
		}
		memset64(GlobalInterruptTable, 0x00, 0xFF * sizeof(nint));
	}

	void RegisterInterrupt(vptr handlerAddress, u16 inter, IdtType interruptType)
	{
		IDTDescEntry* intDescEntry = (IDTDescEntry*)(GlobalIDTR.Offset + inter * sizeof(IDTDescEntry));
		GlobalInterruptTable[inter] = (nint)handlerAddress;
		intDescEntry->TypeAttribs = interruptType;
	}

	forceinline void PanicScreen()
	{
		gConsole.Clear(Color::DarkRed);
		gConsole.SetBackgroundColor(Color::DarkRed);
		gConsole.SetForegroundColor(Color32(0xFFDDDDDD));
	}

	void hDivideByZeroFault()
	{
		PanicScreen();
		Console.WriteLine("ERROR: DIVIDE BY ZERO FAULT!");
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
	}

	void hOverflowTrap()
	{
		PanicScreen();
		Console.WriteLine("Overflow trap");
		halt;
	}

	void hBoundRangeFault()
	{
		PanicScreen();
		Console.WriteLine("Bound Range");
		halt;
	}

	void hInvalidOpcodeFault()
	{
		PanicScreen();
		Console.Write("Invalid Opcode at address: ???");
		halt;
	}

	void hCoprocessorNAFault()
	{
		PanicScreen();
		Console.WriteLine("Coprocessor NA");
		halt;
	}

	void hDoubleFault(nint code)
	{
		Console.Clear(Color::Red);
		Console.SetBackgroundColor(Color::Red);
		Console.SetForegroundColor(Color::White);
		Console.WriteLine("ERROR: DOUBLE FAULT!");
		halt;
	}

	void hCoprocessorSegmentOverrunFault()
	{
		PanicScreen();
		Console.WriteLine("Segment Overrun!");
		halt;
	}

	void hInvalidStateSegmentFault(nint code)
	{
		PanicScreen();
		Console.WriteLine("Invalid State Segment!");
		halt;
	}

	void hSegmentMissingFault(nint code)
	{
		PanicScreen();
		Console.WriteLine("Segment Missing!");
		halt;
	}

	void hStackFault(nint code)
	{
		PanicScreen();
		Console.WriteLine("Stack Exception!");
		halt;
	}

	void hGeneralProtectionFault(nint code)
	{
		PanicScreen();
		Console.WriteLine("ERROR: GENERAL PROTECTION FAULT!");
		halt;
	}

	void hPageFault(nint code)
	{
		PanicScreen();
		Console.WriteLine("ERROR: PAGE FAULT!");
		Console.Write("ATTEMPTED TO ACCESS ADDRESS: ");

		uint64 addr = 0;
		// cr2 contains the page fault linear address (PFLA)
		asm ("mov %0, %%cr2" : "=r" (addr) );

		Console.WriteLine(cstr::ToString(addr, true));
		Console.Write(cstr::format("Error Code: %x0", code));
		halt;
	}

	void hCoprocessorFault()
	{
		PanicScreen();
		Console.WriteLine("ERROR: COPROCESSOR FAULT!");
		halt;
	}

	void hAlignmentCheck(nint code)
	{
		PanicScreen();
		Console.WriteLine("Alignment Check");
		halt;
	}

	void hMachineCheck()
	{
		PanicScreen();
		Console.WriteLine("Machine Check");
		halt;
	}

	void hSIMDFault()
	{
		PanicScreen();
		Console.WriteLine("SIMD Exception");
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

		RTC::EndOfInterrupt();
		if (APIC::InUse) APIC::EndOfInterrupt();
		else PIC::SendEIO(false);
	}

	void hStub()
	{
		auto regs = GetRegisterDump();
		PanicScreen();

		debug("rax: %x0 \nrbx: %x1 \nrcx: %x2 \nrdx: %x3 \n", regs->rax, regs->rbx, regs->rcx, regs->rdx);

		debug("rsi: %x0 \nrdi: %x1 \n", regs->rsi, regs->rdi);

		debug("r8: %x0 \nr9: %x1 \nr10: %x2 \nr11: %x3 \nr12: %x4 \nr13: %x5 \nr14: %x6 \nr15: %x7 \n",
			regs->r8, regs->r9, regs->r10, regs->r11, regs->r12, regs->r13, regs->r14, regs->r15);

		debug("rflags: %x0", regs->rflags);

		halt;
		// if (APIC::InUse) APIC::EndOfInterrupt();
		// else PIC::SendEIO(false);
	}

}
