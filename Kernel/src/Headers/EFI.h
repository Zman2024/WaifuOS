#pragma once
#include <Types.h>

#ifndef H_EFI
#define H_EFI

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