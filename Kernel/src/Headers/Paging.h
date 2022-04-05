#pragma once
#ifndef H_Paging
#define H_Paging
#include <Types.h>

enum struct PTFlag : byte
{
	Present = 0,
	ReadWrite = 1,
	UserSuper = 2,

	WriteThrough = 3,
	CacheDisabled = 4,
	Accessed = 5,

	LargerPages = 7,

	Custom0 = 9,
	Custom1 = 10,
	Custom2 = 11,

	NX = 63

};

struct PageDirectoryEntry
{
	void SetFlag(PTFlag flag, bool value);
	bool GetFlag(PTFlag flag);
	void SetAddress(uint64 Address);
	void SetAddress(void* Address);
	uint64 GetAddress();

	private: uint64 mValue;

};

struct PageTable
{
	PageDirectoryEntry Entries[512];
} attribute((aligned(PAGE_SIZE)));

#endif