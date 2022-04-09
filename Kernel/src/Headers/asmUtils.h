#pragma once
#ifndef H_asmUtils
#define H_asmUtils
#include <Globals.h>


namespace ASM
{
	global void EnableAVX();
	global void TestAVX();
	global nint GetRDRAND();

	global bool CheckXSAVE();
	global void EnableXSAVE();
	// global void memset64(vptr destination, nint value, nint numBytes);
}

#endif