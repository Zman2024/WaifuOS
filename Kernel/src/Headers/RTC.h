#pragma once
#ifndef H_RTC
#define H_RTC
#include <Types.h>
#include <Globals.h>
#include <IOBus.hpp>

namespace RTC
{
	constexpr uint16 PortRegisterSelect = 0x70;
	constexpr uint16 PortCurrentRegister = 0x71;

	constexpr byte RegisterA = 0x0A;
	constexpr byte RegisterB = 0x0B;
	constexpr byte RegisterC = 0x0C;

	constexpr byte BitNMI = 1 << 7;
	constexpr byte BitEnableIRQ = 1 << 6;

	extern volatile uint64 Ticks;
	extern volatile double Time;


	void Initialize(bool intsEnabled = false);
	void Tick();

	byte EndOfInterrupt();
}

#endif