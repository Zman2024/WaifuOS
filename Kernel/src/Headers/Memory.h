#pragma once
#ifndef H_Memory
#define H_Memory
#include <Types.h>
#include <MemoryUtils.hpp>

namespace Memory
{
	struct SegmentHeader
	{
		nint Length;
		SegmentHeader* Next;
		SegmentHeader* Previous;
		bool Free;

		void CombineForward();
		void CombineBackward();
		SegmentHeader* Split(nint splitLength);

	};

	uint64 CalculateMemorySize(vptr map, uint64 nEntries, uint64 descSize);
	void InitializeHeap(vptr heapStart, nint numpages);
	void ExpandHeap(nint length);
	void PrintLeaks();
}

vptr malloc(nint size);
vptr calloc(nint size);
void free(vptr address);

inline vptr operator new(nint size) { return malloc(size); }
inline vptr operator new[](nint size) { return malloc(size); }
inline void operator delete(vptr ptr) { free(ptr); }
inline void operator delete[](vptr ptr) { free(ptr); }
inline void operator delete(vptr ptr, nint sz) { free(ptr); }
inline void operator delete[](vptr ptr, nint sz){ free(ptr); }

#endif