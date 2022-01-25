#include <PageFrameAllocator.h>
#include <MemoryUtils.hpp>

namespace PageFrameAllocator
{
	constexpr auto UseableMemory = 0x07;

	BitMap PageBitmap;

	bool Initialized = false;
	uint64 FreeMemory;
	uint64 AllocatedMemory;
	uint64 ReservedMemory;

	void InitBitmap(u64 size, void* buffer)
	{
		PageBitmap.SizeBytes = size;
		PageBitmap.Buffer = (byte*)buffer;

		memset<u64>(buffer, 0x00, size);
	}

	uint64 LastPageIndex = 0;
	void* RequestPage()
	{
		for (; LastPageIndex < PageBitmap.SizeBytes * 8; LastPageIndex++)
		{
			// Already used
			if (PageBitmap[LastPageIndex]) continue;

			LockPage((void*)(LastPageIndex * PAGE_SIZE));
			return (void*)(LastPageIndex * PAGE_SIZE);
		}

		// Should do an interrupt for pageframe swap before this but i have no ints setup yet
		return nullptr;
	}

	#pragma region Page locking / unlocking

	void FreePage(void* address)
	{
		u64 index = (u64)address / PAGE_SIZE;

		if (!PageBitmap[index]) return;

		if (PageBitmap.Set(index, false))
		{
			FreeMemory += PAGE_SIZE;
			AllocatedMemory -= PAGE_SIZE;
			if (LastPageIndex > index) LastPageIndex = index;
		}
	}

	void LockPage(void* address)
	{
		u64 index = (u64)address / PAGE_SIZE;

		if (PageBitmap[index]) return;

		if (PageBitmap.Set(index, true))
		{
			FreeMemory -= PAGE_SIZE;
			AllocatedMemory += PAGE_SIZE;
		}

	}

	void FreePages(void* address, u64 count)
	{
		for (u64 x = 0; x < count; x++)
		{
			FreePage((void*)((u64)address + (x * PAGE_SIZE)));
		}
	}

	void LockPages(void* address, u64 count)
	{
		for (u64 x = 0; x < count; x++)
		{
			LockPage((void*)((u64)address + (x * PAGE_SIZE)));
		}
	}

	void UnreservePage(void* address)
	{
		u64 index = (u64)address / PAGE_SIZE;

		if (!PageBitmap[index]) return;

		if (PageBitmap.Set(index, false))
		{
			FreeMemory += PAGE_SIZE;
			ReservedMemory -= PAGE_SIZE;
			if (LastPageIndex > index) LastPageIndex = index;
		}
	}

	void ReservePage(void* address)
	{
		u64 index = (u64)address / PAGE_SIZE;

		if (PageBitmap[index]) return;

		if (PageBitmap.Set(index, true))
		{
			FreeMemory -= PAGE_SIZE;
			ReservedMemory += PAGE_SIZE;
		}
	}

	void UnreservePages(void* address, u64 count)
	{
		for (u64 x = 0; x < count; x++)
		{
			UnreservePage((void*)((u64)address + (x * PAGE_SIZE)));
		}
	}

	void ReservePages(void* address, u64 count)
	{
		for (u64 x = 0; x < count; x++)
		{
			ReservePage((void*)((u64)address + (x * PAGE_SIZE)));
		}
	}

	#pragma endregion

	void ReadEfiMemoryMap(EfiMemoryDescriptor* map, u64 mapSize, u64 descSize)
	{
		if (Initialized) return;
		Initialized = true;

		u64 entries = mapSize / descSize;

		u64 largestSizePages = 0;
		void* largestRegionAddress = nullptr;

		for (u64 x = 0; x < entries; x++)
		{
			EfiMemoryDescriptor* desc = (EfiMemoryDescriptor*)((u64)map + (x * descSize));
			switch (desc->type)
			{
				case UseableMemory:
					if (desc->nPages > largestSizePages)
					{
						largestRegionAddress = desc->physAddr;
						largestSizePages = desc->nPages;
					}
					break;
			}
		}

		largestSizePages *= PAGE_SIZE;

		u64 memorySize = Memory::CalculateMemorySize(map, entries, descSize);
		FreeMemory = memorySize;

		u64 bmpSize = (memorySize / PAGE_SIZE / 8) + 1;

		// Init bitmap
		InitBitmap(bmpSize, largestRegionAddress);

		// Reserve all memory
		ReservePages(nullptr, memorySize / PAGE_SIZE);

		for (u64 x = 0; x < entries; x++)
		{
			EfiMemoryDescriptor* desc = (EfiMemoryDescriptor*)((u64)map + (x * descSize));
			switch (desc->type)
			{
				case UseableMemory:
					UnreservePages(desc->physAddr, desc->nPages);
					break;

			}
		}

		// Reserve between 0x00 and 0x100000
		ReservePages(nullptr, 0x100);

		// Lock bitmap
		LockPages(PageBitmap.Buffer, PageBitmap.SizeBytes / PAGE_SIZE + 1);

	}

	uint64 GetFreeRAM()
	{
		return FreeMemory;
	}

	uint64 GetAllocatedRAM()
	{
		return AllocatedMemory;
	}

	uint64 GetReservedRAM()
	{
		return ReservedMemory;
	}

}
