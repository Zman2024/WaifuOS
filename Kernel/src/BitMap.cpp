#include <BitMap.h>

bool BitMap::Get(uint64 index)
{
	if (index > SizeBytes << 3) return false;
	uint64 byteIndex = index >> 3;
	byte bitIndex = index % 8;

	byte bitVal = (1 << 7) >> bitIndex;

	return Buffer[byteIndex] & bitVal;
}

bool BitMap::operator[] (uint64 index)
{
	return Get(index);
}

bool BitMap::Set(uint64 index, bool value)
{
	if (index > SizeBytes << 3) return false;
	uint64 byteIndex = index >> 3;
	byte bitIndex = index % 8;

	byte bitVal = (value << 7) >> bitIndex;

	Buffer[byteIndex] &= ~((1 << 7) >> bitIndex);
	Buffer[byteIndex] |= bitVal;

	return true;
}