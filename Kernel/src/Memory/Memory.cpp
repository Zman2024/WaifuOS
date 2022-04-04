#include <Memory.h>
#include <EFI.h>
#include <PageTableManager.h>
#include <PageFrameAllocator.h>
#include <Globals.h>

namespace Memory
{
	vptr heapBase;
	vptr heapEnd;
	SegmentHeader* LastSegment;
	bool HeapInitialized;

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
		if (sayMessage)
		{
			debug("Note: Alloc #0 is not a 'leak' it's there intentionally.");
			sayMessage = false;
		}

		SegmentHeader* pos = heapBase;
		nint nAllocs = 0;
		while (pos)
		{
			if (!pos->Free)
			{
				debug("Alloc #%0: %x1, Length: %2 bytes", nAllocs++, pos, pos->Length);
			}
			pos = pos->Next;
		}

		if (LastSegment)
			debug("Remaining heap: %x0 bytes", LastSegment->Length);
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

vptr malloc(nint size)
{
	if (!HeapInitialized) return nullptr;

	// bit hacc + cmov go brrrrrrrr
	size -= (size & 0b1111) ? size & 0b1111 : 0x10;
	size += 0x10;

	if (size == 0)
	{
		// cause nullptr pagefult
		asm("mov %%cr2, %0" : : "a" (nullptr));
		intcall(0x0E);
		return nullptr;
	}
	
	SegmentHeader* seg = heapBase;

	while (true)
	{
		if (seg->Free)
		{
			if (seg->Length > size)
			{
				seg->Split(size);
				seg->Free = false;
				return seg + 1;
			}
			if (seg->Length == size)
			{
				seg->Free = false;
				return seg + 1;
			}
		}
		if (seg->Next == nullptr) break;
		seg = seg->Next;
	}

	ExpandHeap(size);
	return malloc(size);
}

vptr calloc(nint size)
{
	vptr ptr = malloc(size);
	if (ptr) memset<u64>(ptr, 0x00, size);
	return ptr;
}

void free(vptr address)
{
	if (!HeapInitialized)
	{
		// cause nullptr pagefult
		asm("mov %%cr2, %0" : : "a" (nullptr));
		intcall(0x0E);
		return nullptr;
	}
	SegmentHeader* seg = (SegmentHeader*)address - 1;
	seg->Free = true;
	seg->CombineForward();
	seg->CombineBackward();
}

void memcpy(vptr dest, const vptr src, uint64 nBytes)
{
	const byte* srcb = (const byte*)src;
	byte* destb = (byte*)dest;

	for (uint64 x = 0; x < nBytes; x++) destb[x] = srcb[x];

}
