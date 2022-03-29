#include <MemoryUtils.hpp>

void memcpy(void* dest, const void* src, uint64 nBytes)
{
	const byte* srcb = (const byte*)src;
	byte* destb = (byte*)dest;

	for (uint64 x = 0; x < nBytes; x++) destb[x] = srcb[x];

}
