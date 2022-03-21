#pragma once
#ifndef H_IOBus
#define H_IOBus
#include <Types.h>

namespace IO
{
	inline void outb(uint16 port, byte value)
	{
		asm ("outb %0, %1" : : "Nd"(port), "a"(value));
	}

	inline byte inb(uint16 port)
	{
		byte value;
		asm ("inb %0, %1" : "=a"(value) : "Nd"(port));
		return value;
	}

	inline void wait()
	{
		asm ("outb 0x80, %%al" : : "a"(0x00));
	}
}


#endif