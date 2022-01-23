#pragma once
#ifndef H_Bitmap
#define H_Bitmap
#include <Types.h>

struct BitMap
{
	size_t SizeBytes;
	byte* Buffer;
	bool operator[] (uint64 index);
	bool Set(uint64 index, bool value);
protected:
	bool Get(uint64 index);
};

#endif