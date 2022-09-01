#pragma once
#ifndef H_APICShared
#define H_APICShared
#include <Types.h>
#include <APICStructs.h>
#include <List.h>

namespace APIC
{
	extern LocalAPIC* LAPIC;
	extern List<IOAPIC*> IOAPICs;
	extern List<IRQRemapRecord*> IRQOverrides;
	extern List<LAPICRecord*> LocalAPICRecords;
	extern List<IOAPICRecord*> IOAPICRecords;

	extern bool UsableAPIC;
	extern bool InUse;

}

#endif // !H_APICShared
