#include <LocalAPIC.h>
#include <Interrupts.h>
#include <PageTableManager.h>
#include <PageFrameAllocator.h>

namespace APIC
{
	void InitializeLAPIC(int cpu)
	{
		LAPIC = PageFrameAllocator::RequestPage<LocalAPIC>();
		PageTableManager::MapMemory(LAPIC, (vptr)PhysicalLAPIC);
		PageFrameAllocator::LockPage((vptr)PhysicalLAPIC);

		LAPIC->TaskPriorityRegister = 0x00; // enable all ints
		LAPIC->DestinationFormatRegister = -1; // flat
		LAPIC->LogicalDestinationRegister = cpu << 24; // set logical CPU for interrupts

		LAPIC->LVTTimerRegister = 0x10000; // disables the timer interrupt
		LAPIC->LVTPerformanceMonitoringCountersRegister = 0x10000; // disables pmc interrupt
		LAPIC->SpuriousInterruptVectorRegister = 0x00; // not turned on just yet :lenny:

		debug("Local APIC Initialized!");
	}
}