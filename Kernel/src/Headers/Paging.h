#pragma once
#ifndef H_Paging
#define H_Paging
#include <Types.h>

enum struct PTFlag : uint64
{
	Present			=	1 << 0,
	ReadWrite		=	1 << 1,
	UserSuper		=	1 << 2,
						  	 
	WriteThrough	=	1 << 3,
	CacheDisabled	=	1 << 4,
	Accessed		=	1 << 5,
						  	 
	LargerPages		=	1 << 7,
						  	 
	Custom0			=	1 << 9,
	Custom1			=	1 << 10,
	Custom2			=	1 << 11,
						  	 
	NX				=	1 << 63,

};

struct PageDirectoryEntry
{
	forceinline void SetFlag(PTFlag flag, bool value)
	{
		mValue &= ~((uint64)flag);
		mValue |= ((uint64)flag) & 0xFFFFFFFFFFFFFFFF;
	}

	forceinline bool GetFlag(PTFlag flag) { return (mValue & (uint64)flag) != 0; }

	forceinline void SetAddress(uint64 Address)
	{
		// Normalize address to fit into the 40 bits
		Address &= 0x000000FFFFFFFFFF;

		// Clear the address that was present
		mValue &= 0xFFF0000000000FFF;

		// Set the address bits
		mValue |= Address << 12;
	}

	forceinline void SetAddress(void* Address)
	{
		SetAddress((u64)Address);
	}

	forceinline uint64 GetAddress()
	{
		return (mValue & 0x000FFFFFFFFFF000) >> 12;
	}

	private: uint64 mValue;

};

struct PageTable
{
	PageDirectoryEntry Entries[512];
} attribute((aligned(PAGE_SIZE)));

#endif