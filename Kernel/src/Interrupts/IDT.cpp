#include <IDT.h>

void IDTDescEntry::SetOffset(uint64 offset)
{
	this->Offset0 = u16(offset & 0xFFFF);
	this->Offset1 = u16((offset & 0xFFFF0000) >> 16);
	this->Offset2 = u32((offset & 0xFFFFFFFF00000000) >> 32);
}

uint64 IDTDescEntry::GetOffset()
{
	u64 value = (u64)(Offset0 & 0xFFFF);
	value |= ((u64)Offset1) << 16;
	value |= ((u64)Offset2) << 32;
	return value;
}

