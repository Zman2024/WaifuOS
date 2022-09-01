#pragma once
#ifndef H_IDT
#define H_IDT
#include <Types.h>

enum struct IdtType : byte
{
	InterruptGate = 0b10001110,
	TrapGate = 0b10001111,
};


struct SegmentSelector
{
	forceinline SegmentSelector() {  }
	forceinline SegmentSelector(uint16 value) { *(uint16*)this = value; }
	forceinline SegmentSelector(uint16 index, byte privilegeLevel, bool UseLDT)
	{
		DTIndex = index;
		PrivilegeLevel = privilegeLevel & 0b11;
		UseLDT = (UseLDT == true);
	}

	forceinline operator uint16() { return *(uint16*)this; }

	uint16 PrivilegeLevel : 2;
	uint16 UseLDT : 1;
	uint16 DTIndex : 13;

} packed;

struct IDTDescEntry
{
	uint16 Offset0;
	SegmentSelector Selector; // Segment Selector in GTD / LDT

	byte IST;
	IdtType TypeAttribs;

	uint16 Offset1;

	uint32 Offset2;
	uint32 rsv0;

	void SetOffset(uint64 offset);
	uint64 GetOffset();
};

struct IDTR
{
	uint16 Limit;
	uint64 Offset;
} packed;


#endif