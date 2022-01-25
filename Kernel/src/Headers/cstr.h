#pragma once
#ifndef H_cstr
#define H_cstr
#include <Types.h>

namespace cstr
{
	// Get length of cstring
	uint64 Length(const char* str);

	// returns true if str0 and str1 are the same
	bool strcmp(const char* str0, const char* str1);

	// returns true if str0 and str1 are the same
	bool strcmp(const char* str0, const char* str1, u64 len);

	// Reverse a cstring
	void Reverse(char* str, uint64 length = 0);

	// Convert a number to a cstring
	char* ToString(int64 num, bool hex = false);

	// Convert a cstring to an int
	int64 ToInt(const char* str, bool hex = false);
}

#endif
