#pragma once
#ifndef H_cstr
#define H_cstr
#include <Types.h>

namespace cstr
{
	// Get length of cstring
	uint64 len(const char* str);

	// Reverse a cstring
	void Reverse(char* str, uint64 length = 0);

	// Convert a number to a cstring
	char* ToString(int64 num, bool hex = false);

	// Convert a cstring to an int
	int64 ToInt(const char* str, bool hex = false);
}

#endif
