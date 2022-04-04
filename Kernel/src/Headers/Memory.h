#pragma once
#ifndef H_Memory
#define H_Memory
#include <Types.h>

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

template <typename T> inline void memset(void* address, T value, uint64 nBytes)
{
	T* tAddr = (T*)address;
	uint16 rem = nBytes % sizeof(T);
	nBytes = nBytes / sizeof(T);
	for (uint64 x = 0; x < nBytes; x++)
	{
		*tAddr = value;
		tAddr++;
	}

	byte* cst = (byte*)&value;
	for (uint16 x = 0; x < rem; x++)
	{
		((byte*)tAddr)[x] = cst[x];
	}

}

void memcpy(vptr dest, const vptr src, uint64 nBytes);

inline vptr operator new(nint size) { return malloc(size); }
inline vptr operator new[](nint size) { return malloc(size); }
inline void operator delete(vptr ptr) { free(ptr); }
inline void operator delete[](vptr ptr) { free(ptr); }
inline void operator delete(vptr ptr, nint sz) { free(ptr); }
inline void operator delete[](vptr ptr, nint sz){ free(ptr); }

#endif