#pragma once
#ifndef H_IDT
#define H_IDT
#include <Types.h>

enum struct IdtType : byte
{
	InterruptGate = 0b10001110,
	CallGate = 0b10001100,
	TrapGate = 0b10001111,
};

struct IDTDescEntry
{
	uint16 Offset0;
	uint16 Selector; // Code segment selector in GTD / LDT

	byte IST;
	IdtType TypeAttribs;

	uint16 Offset1;

	uint32 Offset2;
	uint32 Ignore;

	void SetOffset(uint64 offset);
	uint64 GetOffset();
};

struct IDTR
{
	uint16 Limit;
	uint64 Offset;
} attribute((packed));


#endif