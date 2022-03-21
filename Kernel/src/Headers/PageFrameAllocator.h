#pragma once
#ifndef H_PageFrameAllocator
#define H_PageFrameAllocator
#include <Types.h>
#include <Structs.h>
#include <BitMap.h>
#include <Memory.h>

// namespace cause i only plan on having one of these
namespace PageFrameAllocator
{
	void ReadEfiMemoryMap(EfiMemoryDescriptor* map, u64 mapSize, u64 descSize);

	vptr RequestPage();
	vptr RequestPages(u64 pages);

	void FreePage(void* address);
	void LockPage(void* address);

	void FreePages(void* address, u64 count);
	void LockPages(void* address, u64 count);

	uint64 GetFreeRAM();
	uint64 GetAllocatedRAM();
	uint64 GetReservedRAM();

	template <typename T>
	static inline T* RequestPage()
	{
		return (T*)RequestPage();
	}

	template <typename T>
	static inline T* RequestPages(u64 pages)
	{
		return (T*)RequestPages(pages);
	}

}

#endif