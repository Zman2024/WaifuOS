#pragma once
#ifndef H_cstr
#define H_cstr
#include <Types.h>
#include <string.h>

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

	// returns -1 if not found
	int64 IndexOf(const char* str, char character, nint len);

	// returns -1 if not found
	forceinline int64 IndexOf(const char* str, char character) { return IndexOf(str, character, strlen(str)); }

	// FUCK THIS SHIT KILL ME
	char* format(const char * str, int64 p0, int64 p1 = 0, int64 p2 = 0, int64 p3 = 0, int64 p4 = 0, int64 p5 = 0, int64 p6 = 0, int64 p7 = 0, int64 p8 = 0, int64 p9 = 0);

	// Convert a cstring to an int
	int64 ToInt(const char* str, bool hex = false);

	// A very lazy conversion from wchar (UTF-16 LE) to ASCII
	char* wstrTocstr(const wchar* wstr, nint len);

	// A very lazy conversion from wchar (UTF-16 LE) to ASCII
	wchar* cstrTowstr(const char* cstr, nint len);

	void wstrncpy(wchar* dest, const wchar* src, nint max);

	// Length of a zero terminated wstring
	nint wstrlen(const wchar* str);

	// returns -1 if not found
	int64 wstrIndexOf(const wchar* str, wchar character);

	bool wstrcmp(const wchar* str0, const wchar* str1);

	bool wstrcmp(const wchar* str0, const wchar* str1, nint len);

	// returns allocated memory, remember to free it!
	// endIndex is EXCLUSIVE
	char* substring(const char* str, nint startIndex, nint endIndex);

	// returns allocated memory, remember to free it!
	// endIndex is EXCLUSIVE
	wchar* wsubstring(const wchar* str, nint startIndex, nint endIndex);

	// Makes all the characters in a cstr lowercase
	void ToLower(char* str);

	// Makes all the characters in a cstr uppercase
	void ToUpper(char* str);

	// Makes all the characters in a wstr lowercase (kinda)
	void wstrToLower(wchar* str);

}

#endif
