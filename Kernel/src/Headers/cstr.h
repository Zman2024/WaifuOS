#pragma once
#include <Types.h>

#ifndef H_cstr
#define H_cstr

namespace cstr
{
	uint64 len(const char* str);
	void Reverse(char* str, uint64 length = 0);
	char* ToString(int64 num, bool hex = false);
	int64 ToInt(const char* str, bool hex = false);

}

#endif
