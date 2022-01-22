#pragma once
#include <Types.h>
#include <Structs.h>
#include <BitMap.h>
#include <Memory.h>

#ifndef H_PageFrameAllocator
#define H_PageFrameAllocator

// namespace cause i only plan on having one of these
namespace PageFrameAllocator
{
	void ReadEfiMemoryMap(EfiMemoryDescriptor* map, u64 mapSize, u64 descSize);

	void* RequestPage();

	void FreePage(void* address);
	void LockPage(void* address);

	void FreePages(void* address, u64 count);
	void LockPages(void* address, u64 count);

	uint64 GetFreeRAM();
	uint64 GetAllocatedRAM();
	uint64 GetReservedRAM();

	template <typename T>
	T* RequestPage()
	{
		return (T*)RequestPage();
	}

}

#endif