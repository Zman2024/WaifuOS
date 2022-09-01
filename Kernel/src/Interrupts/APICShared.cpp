#include <APICShared.h>

namespace APIC
{
	LocalAPIC* LAPIC;
	List<IOAPIC*> IOAPICs;
	List<IRQRemapRecord*> IRQOverrides;
	List<LAPICRecord*> LocalAPICRecords;
	List<IOAPICRecord*> IOAPICRecords;

	bool UsableAPIC = false;
	bool InUse = false;
}
