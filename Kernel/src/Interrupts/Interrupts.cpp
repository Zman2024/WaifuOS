#include <Interrupts.h>

namespace Interrupts
{
	void RegisterInterrupt(void* handlerAddress, byte interrupt, IdtType interruptType)
	{
		IDTDescEntry* intDescEntry = (IDTDescEntry*)(GlobalIDTR.Offset + interrupt * sizeof(IDTDescEntry));
		intDescEntry->SetOffset((u64)handlerAddress);
		intDescEntry->TypeAttribs = interruptType;
		intDescEntry->Selector = 0x08;
	}

	void LoadIDT()
	{
		asm ("lidt %0" : : "m" (GlobalIDTR));
	}

	void PanicScreen()
	{
		gConsole.Clear(Color::DarkRed);
		gConsole.SetBackgroundColor(Color::DarkRed);
		gConsole.SetForegroundColor(Color(0xFFDDDDDD));
	}

	void hDivideByZeroFault(struct InterruptFrame* frame)
	{
		PanicScreen();
		gConsole.WriteLine("ERROR: DIVIDE BY ZERO FAULT!");
		halt;
	}

	void hDoubleFault(struct InterruptFrame* frame)
	{
		gConsole.Clear(Color::Red);
		gConsole.SetBackgroundColor(Color::Red);
		gConsole.SetForegroundColor(Color::White);
		gConsole.WriteLine("ERROR: DOUBLE FAULT!");
		halt;
	}

	void hGeneralProtectionFault(struct InterruptFrame* frame)
	{
		PanicScreen();
		gConsole.WriteLine("ERROR: GENERAL PROTECTION FAULT!");
		halt;
	}

	void hPageFault(struct InterruptFrame* frame)
	{
		PanicScreen();
		gConsole.WriteLine("ERROR: PAGE FAULT!");
		halt;
	}

	void hCoprocessorFault(struct InterruptFrame* frame)
	{
		PanicScreen();
		gConsole.WriteLine("ERROR: COPROCESSOR FAULT!");
		halt;
	}

}
