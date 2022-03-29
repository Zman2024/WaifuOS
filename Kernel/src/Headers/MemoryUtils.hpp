#pragma once
#ifndef H_MemoryUtils
#define H_MemoryUtils
#include <Types.h>

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

void memcpy(void* dest, const void* src, uint64 nBytes);

// Memory.asm

global nint strlen(const char* str);

global void strcpy(char* dest, const char* src);

global void strcpys(char* dest, const char* src, nint max);

#endif