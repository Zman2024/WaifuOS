#pragma once
#include <Types.h>
#include <EFI.h>
#ifndef H_Memory
#define H_Memory

namespace Memory
{
	uint64 CalculateMemorySize(EfiMemoryDescriptor* map, uint64 nEntries, uint64 descSize);
}

#endif