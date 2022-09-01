#include <LocalAPIC.h>
#include <Interrupts.h>
#include <PageTableManager.h>
#include <PageFrameAllocator.h>

namespace APIC
{
	void InitializeLAPIC(int cpu)
	{
		if (!LAPIC)
		{
			error("wtf? no lapic ptr?");
			OS_HLT;
		}

		PageTableManager::MapMemory(LAPIC, LAPIC);
		PageFrameAllocator::LockPage(LAPIC);

		LAPIC->TaskPriorityRegister = 0x00; // enable all ints
		LAPIC->DestinationFormatRegister = -1; // flat
		LAPIC->LogicalDestinationRegister = cpu << 24; // set logical CPU for interrupts

		// disable all lvt ints
		LAPIC->LVTCorrectedMachineCheckInterruptRegister |= 0x10000; // disable machine check interrupt (lol)
		LAPIC->LVTTimerRegister |= 0x10000; // disables the timer interrupt
		LAPIC->LVTThermalSensorRegister |= 0x10000; // disables themal sensor
		LAPIC->LVTPerformanceMonitoringCountersRegister |= 0x10000; // disables pmc interrupt
		LAPIC->LVTLINT0Register |= 0x10000; // bruh
		LAPIC->LVTLINT1Register |= 0x10000; // disables lint1
		LAPIC->LVTErrorRegister |= 0x10000; // disabled moment

		LAPIC->SpuriousInterruptVectorRegister = 0x00; // not turned on just yet :lenny:

		debug("Local APIC Initialized!");
	}
}