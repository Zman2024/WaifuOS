#pragma once
#ifndef H_IOAPIC
#define H_IOAPIC
#include <Types.h>
#include <APICStructs.h>
#include <Interrupts.h>

namespace APIC
{
	inline byte GetRemappedIRQ(byte irq)
	{
		if (IRQOverrides[irq] && IRQOverrides[irq]->IrqSource == irq)
		{
			return IRQOverrides[irq]->InterruptVector;
		}
		return irq;
	}

	inline bool SetEntry(byte irq, RedirectionEntry entry)
	{
		for (byte x = 0; x < IOAPICsCount; x++)
		{
			auto ioapic = IOAPICs + x;
			if (ioapic->GetInterruptBase() > irq) continue;
			if (ioapic->GetInterruptBase() + ioapic->GetMaxRedirectionEntries() < irq) continue;

			irq = GetRemappedIRQ(irq);
			ioapic->WriteIOAPICRedirectionEntry(irq, entry);
			return true;
		}
		return false;
	}

	inline void SetEntry(byte irqVector)
	{
		RedirectionEntry entry = irqVector;
		SetEntry(irqVector - Interrupts::IRQ_OFFSET, entry);
	}

	inline void InitializeIOAPIC()
	{
		IOAPICsCount = 0x00;
		for (byte x = 0; x < IOAPICRecordsCount; x++)
		{
			auto record = IOAPICRecords[x];
			IOAPICs[x] = IOAPIC(record);
			debug("Initialized IOAPIC:%x0", IOAPICs[x].GetID());
			IOAPICsCount++;
		}
	}

}

#endif