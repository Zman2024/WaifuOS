#pragma once
#ifndef H_LocalAPIC
#define H_LocalAPIC
#include <Types.h>
#include <APICStructs.h>
#include <APIC.h>

namespace APIC
{
	void InitializeLAPIC(int cpu = 0);
}

#endif