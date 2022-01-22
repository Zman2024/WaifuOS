#pragma once
#include <Types.h>

#ifndef H_MemoryUtils
#define H_MemoryUtils

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

void memcpy(const void* src, void* dest, uint64 nBytes);

#endif