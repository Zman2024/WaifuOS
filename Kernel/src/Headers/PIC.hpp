#pragma once
#ifndef H_PIC
#define H_PIC
#include <Types.h>
#include <IOBus.hpp>
#include <Interrupts.h>

namespace PicMask
{
	constexpr uint16 PIT = 0b00000001;
	constexpr uint16 Keyboard = 0b00000010;
	constexpr uint16 SlaveEnabled = 0b00000100;
	constexpr uint16 COM2 = 0b00001000;
	constexpr uint16 COM1 = 0b00010000;
	constexpr uint16 LPT2 = 0b00100000;
	constexpr uint16 Floppy = 0b01000000;
	constexpr uint16 LPT1 = 0b10000000;

	constexpr uint16 RTC = 0b00000001 << 8;
	constexpr uint16 Unassigned0 = 0b00000010 << 8;
	constexpr uint16 Unassigned1 = 0b00000100 << 8;
	constexpr uint16 Unassigned2 = 0b00001000 << 8;
	constexpr uint16 Mouse = 0b00010000 << 8;
	constexpr uint16 MathCoproc = 0b00100000 << 8;
	constexpr uint16 HDD0 = 0b01000000 << 8;
	constexpr uint16 HDD1 = 0b10000000 << 8;

}

namespace PIC
{
	constexpr byte PIC_MASTER_OFFSET = Interrupts::IRQ_OFFSET;
	constexpr byte PIC_SLAVE_OFFSET = PIC_MASTER_OFFSET + 8;

	constexpr byte PicEoi = 0x20;

	constexpr byte MasterCommand = 0x20;
	constexpr byte MasterData = 0x21;

	constexpr byte SlaveCommand = 0xA0;
	constexpr byte SlaveData = 0xA1;

	void Remap();
	void Mask(uint16 mask, bool state);

	inline void Disable()
	{
		IO::outb(MasterData, 0xFF);
		IO::wait();
		IO::outb(SlaveData, 0xFF);
		IO::wait();
	}

	inline void SendEIO(bool slave)
	{
		if (slave) IO::outb(SlaveCommand, PicEoi);
		IO::outb(MasterCommand, PicEoi);
	}

}

#endif