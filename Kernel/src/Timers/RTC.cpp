#include <RTC.h>

namespace RTC
{

	volatile uint64 Ticks = 0;
	volatile double Time = 0;

	void Initialize(bool intsEnabled)
	{
		cli;
		using namespace IO;
		outb(PortRegisterSelect, RegisterB | BitNMI);
		byte prev = inb(PortCurrentRegister);
		outb(PortRegisterSelect, RegisterB | BitNMI);
		outb(PortCurrentRegister, prev | BitEnableIRQ);

		// Just to make sure it will fire ints
		EndOfInterrupt();
		if (intsEnabled) sti;
	}

	void Tick()
	{
		Ticks++;
		// For some reason QEMU's RTC doesnt fire at 1024hz
		// Its somewhere closer to 691.5hz ish
		Time += 1.0 / 1024.0;
	}

	byte EndOfInterrupt()
	{
		IO::outb(PortRegisterSelect, RegisterC);
		return IO::inb(PortCurrentRegister);
	}

}