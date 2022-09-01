#pragma once
#ifndef H_IOAPIC
#define H_IOAPIC
#include <Types.h>
#include <APICStructs.h>
#include <Interrupts.h>
#include <APICShared.h>

namespace APIC
{
	inline byte GetRemappedIRQ(byte irq)
	{
		if (IRQOverrides[irq] && IRQOverrides[irq]->IrqSource == irq)
		{
			return IRQOverrides[irq]->IrqOverride;
		}
		return irq;
	}

	inline bool SetEntry(byte irq, RedirectionEntry entry)
	{
		for (byte x = 0; x < IOAPICs.GetCount(); x++)
		{
			auto ioapic = IOAPICs[x];
			if (ioapic->GetInterruptBase() > irq) continue;
			if (ioapic->GetInterruptBase() + ioapic->GetMaxRedirectionEntries() < irq) continue;

			irq = GetRemappedIRQ(irq);
			ioapic->WriteIOAPICRedirectionEntry(irq, entry);
			return true;
		}
		return false;
	}

	forceinline void SetEntry(byte irqVector)
	{
		RedirectionEntry entry = irqVector;
		SetEntry(irqVector - Interrupts::IRQ_OFFSET, entry);
	}

	inline void InitializeIOAPIC()
	{
		for (byte x = 0; x < IOAPICRecords.GetCount(); x++)
		{
			auto record = IOAPICRecords.Get(x);
			IOAPICs.Add(new IOAPIC(record));
		}
	}

}

#endif