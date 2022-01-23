#include <Paging.h>

void PageDirectoryEntry::SetFlag(PTFlag flag, bool value)
{
	mValue &= ~((uint64)1 << (uint64)flag);
	mValue |= (uint64)value << (uint64)flag;
}

bool PageDirectoryEntry::GetFlag(PTFlag flag)
{
	return (mValue & (uint64)1 << (uint64)flag) >> (uint64)flag;
}

void PageDirectoryEntry::SetAddress(uint64 Address)
{
	// Normalize address to fit into the 40 bits
	Address &= 0x000000FFFFFFFFFF;

	// Clear the address that was present
	mValue &= 0xFFF0000000000FFF;

	// Set the address bits
	mValue |= Address << 12;
}

void PageDirectoryEntry::SetAddress(void* Address)
{
	SetAddress((u64)Address);
}

uint64 PageDirectoryEntry::GetAddress()
{
	return (mValue & 0x000FFFFFFFFFF000) >> 12;
}
