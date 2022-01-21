#pragma once
#ifndef H_EFI
#define H_EFI
#include "Types.h"

struct EfiMemoryDescriptor
{
	uint32 type;
	uint32 pad;
	void* physAddr;
	void* virtAddr;
	uint64 nPages;
	uint64 attribs;
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];
#endif