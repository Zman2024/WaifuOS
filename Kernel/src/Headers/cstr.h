#pragma once
#ifndef H_cstr
#define H_cstr
#include <Types.h>

namespace cstr
{
	// returns true if str0 and str1 are the same
	bool strcmp(const char* str0, const char* str1);

	// returns true if str0 and str1 are the same
	bool strcmp(const char* str0, const char* str1, u64 len);

	// Reverse a cstring
	void Reverse(char* str, uint64 length = 0);

	// Convert a number to a cstring
	char* ToString(int64 num, bool hex = false);

	// FUCK THIS SHIT KILL ME
	char* format(const char * str, int64 p0, int64 p1 = 0, int64 p2 = 0, int64 p3 = 0, int64 p4 = 0, int64 p5 = 0, int64 p6 = 0, int64 p7 = 0, int64 p8 = 0, int64 p9 = 0);

	// Convert a cstring to an int
	int64 ToInt(const char* str, bool hex = false);

	// A very lazy conversion from wchar (UTF-16 LE) to ASCII
	char* wstrTocstr(const wchar* wstr, nint len);

	void wstrncpy(wchar* dest, const wchar* src, nint max);

}

#endif
