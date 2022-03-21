#pragma once
#ifndef H_Memory
#define H_Memory
#include <Types.h>
#include <EFI.h>

namespace Memory
{
	uint64 CalculateMemorySize(EfiMemoryDescriptor* map, uint64 nEntries, uint64 descSize);
}



#endif