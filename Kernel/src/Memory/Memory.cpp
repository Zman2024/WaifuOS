#include <Memory.h>
#include <EFI.h>
#include <PageTableManager.h>
#include <PageFrameAllocator.h>
#include <Globals.h>
#include <Synchronization.hpp>

namespace Memory
{
	vptr heapBase;
	vptr heapEnd;
	SegmentHeader* LastSegment;
	bool HeapInitialized = false;

	uint64 CalculateMemorySize(vptr map, uint64 nEntries, uint64 descSize)
	{
		u64 memorySize = 0;

		for (u64 x = 0; x < nEntries; x++)
		{
			EfiMemoryDescriptor* desc = (EfiMemoryDescriptor*)(((u64)map) + (x * descSize));
			memorySize += desc->nPages * PAGE_SIZE;
		}

		return memorySize;
	}

	void PrintLeaks()
	{
		static bool sayMessage = true;

		debug("========== Current Memory Allocations ==========");

		if (sayMessage)
		{
			debug("Note: Alloc #0 is not a 'leak' it's there intentionally.");
			sayMessage = false;
		}

		SegmentHeader* pos = heapBase;
		if (!pos)
		{
			error("SOMETHING HAS GONE VERY WRONG (heapBase = nullptr?)");
			OS_HLT;
		}

		nint nAllocs = 0;
		nint nFreeBytes = 0;
		while (pos && pos >= heapBase && pos <= heapEnd)
		{
			if (!pos->Free)
			{
				debug("Alloc #%0: %x1, Length: %2 bytes", nAllocs++, pos, pos->Length);
			}
			else nFreeBytes += pos->Length;
			pos = pos->Next;
		}

		if (LastSegment)
			debug("Remaining heap: %x0 bytes", LastSegment->Length + nFreeBytes);
	}

	nint GetRemainingHeap()
	{
		SegmentHeader* pos = heapBase;
		if (!pos)
		{
			error("SOMETHING HAS GONE VERY WRONG (heapBase = nullptr?)");
			OS_HLT;
		}

		nint nFreeBytes = 0;
		while (pos && pos >= heapBase && pos <= heapEnd)
		{
			if (pos->Free)
			{
				nFreeBytes += pos->Length;
			}
			pos = pos->Next;
		}

		if (LastSegment) return LastSegment->Length + nFreeBytes;
		return nFreeBytes;
	}

	void InitializeHeap(vptr heapStart, nint pages)
	{
		if (HeapInitialized) return;

		debug("Initializing Heap...");
		heapBase = heapStart;

		nint temp = (nint)heapStart;
		
		for (nint x = 0; x < pages; x++)
		{
			PageTableManager::MapMemory((vptr)temp, PageFrameAllocator::RequestPage());
			memset64((vptr)temp, 0x00, PAGE_SIZE);
			temp += PAGE_SIZE;
		}

		nint heapLength = pages * PAGE_SIZE;

		heapEnd = (vptr)(nint(heapStart) + heapLength);
		
		SegmentHeader* header = heapStart;
		header->Length = heapLength - sizeof(SegmentHeader);
		header->Next = nullptr;
		header->Previous = nullptr;
		header->Free = true;

		LastSegment = header;
		HeapInitialized = true;

		// we leak the first allocation address so that our
		// entire heap cant be deallocated
		malloc(0x10);
	}

	void ExpandHeap(nint length)
	{
		if (!HeapInitialized)
		{
			error("Ran out of early memory!");
			OS_HLT;
		}

		if (length % PAGE_SIZE)
		{
			length -= length % PAGE_SIZE;
			length += PAGE_SIZE;
		}

		SegmentHeader* newSegment = heapEnd;
		nint pageCount = length / PAGE_SIZE;

		for (nint x = 0; x < pageCount; x++)
		{
			PageTableManager::MapMemory(heapEnd, PageFrameAllocator::RequestPage());
			memset64(heapEnd, 0x00, PAGE_SIZE);
			heapEnd = nint(heapEnd) + PAGE_SIZE;
		}

		newSegment->Free = true;
		newSegment->Previous = LastSegment;
		LastSegment->Next = newSegment;
		LastSegment = newSegment;
		newSegment->Next = nullptr;
		newSegment->Length = length - sizeof(SegmentHeader);
		newSegment->CombineBackward();

	}

	void SegmentHeader::CombineForward()
	{
		if (Next == nullptr) return;
		if (!Next->Free) return;

		this->Length += (this->Next->Length + sizeof(SegmentHeader));

		if (Next == LastSegment) LastSegment = this;
		if (Next->Next != nullptr) Next->Next->Previous = this;

		this->Next = Next->Next;
	}

	void SegmentHeader::CombineBackward()
	{
		if (Previous != nullptr && Previous->Free) Previous->CombineForward();
	}

	SegmentHeader* SegmentHeader::Split(nint splitLength)
	{
		if (splitLength < 0x10) return nullptr;
		i64 newLength = Length - splitLength - sizeof(SegmentHeader);
		if (newLength < 0x10) return nullptr;
		SegmentHeader* newHeader = nint(this) + splitLength + sizeof(SegmentHeader);

		// haha nullptr go brrrrrrrrr
		if (this->Next) this->Next->Previous = newHeader;
		newHeader->Next = this->Next;
		this->Next = newHeader;
		newHeader->Previous = this;
		newHeader->Length = newLength;
		newHeader->Free = this->Free;
		this->Length = splitLength;

		if (LastSegment == this) LastSegment = newHeader;
		return newHeader;
	}

}

using namespace Memory;

constexpr nint earlyMallocSize = 0x4000;
byte* earlyMallocBase[earlyMallocSize]; // 4 pages

vptr earlyMalloc(nint size)
{
	static bool earlyMallocInit = false;
	static nint baseOffset = 0;

	if (!earlyMallocInit)
	{
		memset64(earlyMallocBase, 0x00, earlyMallocSize);
		earlyMallocInit = true;
	}

	size -= (size & 0b1111) ? size & 0b1111 : 0x10;
	size += 0x10;

	if (size == 0) return nullptr;

	vptr alloc = vptr(earlyMallocBase + baseOffset);
	baseOffset += size;

	return alloc;
}

SpinLock memoryLock = SpinLock();
vptr malloc(nint size)
{
	if (!HeapInitialized) return earlyMalloc(size);

	// bit hacc + cmov go brrrrrrrr
	size -= (size & 0b1111) ? size & 0b1111 : 0x10;
	size += 0x10;

	if (size == 0)
	{
		// cause nullptr page fault
		*(byte*)nullptr;
		return nullptr;
	}
	
	memoryLock.Aquire();

	SegmentHeader* seg = heapBase;

	while (true)
	{
		if (seg->Free)
		{
			if (seg->Length > size)
			{
				seg->Split(size);
				seg->Free = false;
				memoryLock.Free();
				return seg + 1;
			}
			if (seg->Length == size)
			{
				seg->Free = false;
				memoryLock.Free();
				return seg + 1;
			}
		}
		if (seg->Next == nullptr) break;
		seg = seg->Next;
	}

	ExpandHeap(size);

	memoryLock.Free();
	return malloc(size);
}

vptr calloc(nint size)
{
	size -= (size & 0b1111) ? size & 0b1111 : 0x10;
	size += 0x10;

	vptr ptr = malloc(size);
	if (ptr) memset64(ptr, 0x00, size);
	return ptr;
}

void free(vptr address)
{
	if (!address)
	{
		// cause nullptr pagefult
		*(byte*)nullptr;
		return nullptr;
	}

	if (!HeapInitialized)
	{
		return;
	}

	if (address >= earlyMallocBase && address < earlyMallocBase + earlyMallocSize) return;

	memoryLock.Aquire();
	SegmentHeader* seg = (SegmentHeader*)address - 1;
	memset64(address, 0x33, seg->Length);
	seg->Free = true;
	seg->CombineForward();
	seg->CombineBackward();
	memoryLock.Free();
}

