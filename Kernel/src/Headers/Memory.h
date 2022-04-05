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

global vptr malloc(nint size);
global vptr calloc(nint size);
global void free(vptr address);

template <typename T> inline void memset(void* address, T value, const uint64 nBytes)
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

inline void memset64(vptr address, uint64 value, uint64 bytes)
{
	register byte remainder = bytes & 0b111;
	register uint64 rounds = bytes >> 3; // divide by 8
	register nint indexer = 0;

	while (indexer < rounds) ((u64*)address)[indexer++] = value;

	if (remainder == 0) return;

	indexer <<= 3;
	rounds <<= 3;
	rounds += remainder;

	register nint indexer0 = 0;
	while (indexer < rounds) ((byte*)address)[indexer++] = (byte)(value >> (indexer0++ << 3));
}

void memcpy(vptr dest, const vptr src, uint64 nBytes);

inline vptr operator new(nint size) { return malloc(size); }
inline vptr operator new[](nint size) { return malloc(size); }
inline void operator delete(vptr ptr) { free(ptr); }
inline void operator delete[](vptr ptr) { free(ptr); }
inline void operator delete(vptr ptr, nint sz) { free(ptr); }
inline void operator delete[](vptr ptr, nint sz){ free(ptr); }

#endif