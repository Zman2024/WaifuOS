#include <Memory.h>

namespace Memory
{
	uint64 CalculateMemorySize(EfiMemoryDescriptor* map, uint64 nEntries, uint64 descSize)
	{
		u64 memorySize = 0;

		for (u64 x = 0; x < nEntries; x++)
		{
			EfiMemoryDescriptor* desc = (EfiMemoryDescriptor*)(((u64)map) + (x * descSize));
			memorySize += desc->nPages * PAGE_SIZE;
		}

		return memorySize;
	}
}