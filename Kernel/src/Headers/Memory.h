#pragma once
#ifndef H_Memory
#define H_Memory
#include <Types.h>

namespace Memory
{
	struct SegmentHeader
	{
		nint Length = 0x00;
		SegmentHeader* Next = nullptr;
		SegmentHeader* Previous = nullptr;
		bool Free = false;

		void CombineForward();
		void CombineBackward();
		SegmentHeader* Split(nint splitLength);

	};

	uint64 CalculateMemorySize(vptr map, uint64 nEntries, uint64 descSize);
	void InitializeHeap(vptr heapStart, nint numpages);
	void ExpandHeap(nint length);
	void PrintLeaks();
}

global vptr malloc(nint size);
global vptr calloc(nint size);
global void free(vptr address);

template <typename T> 
inline void memset(void* address, T value, const uint64 nBytes)
{
	register T* tAddr = (T*)address;
	const uint16 rem = nBytes % sizeof(T);
	const register nint nRounds = nBytes / sizeof(T);

	for (register nint x = 0; x < nRounds; x++)
	{
		*tAddr = value;
		tAddr++;
	}

	byte* cst = (byte*)&value;
	for (register uint16 x = 0; x < rem; x++)
	{
		((byte*)tAddr)[x] = cst[x];
	}

}

// asmUtils.asm
global void memset64(vptr destination, nint value, nint numBytes);

void memcpy(vptr dest, const vptr src, uint64 nBytes);

forceinline vptr operator new(nint size)				{ return malloc(size); }
forceinline vptr operator new[](nint size)				{ return malloc(size); }
forceinline void operator delete(vptr ptr)				{ free(ptr); }
forceinline void operator delete[](vptr ptr)			{ free(ptr); }
forceinline void operator delete(vptr ptr, nint sz)		{ free(ptr); }
forceinline void operator delete[](vptr ptr, nint sz)	{ free(ptr); }

#endif