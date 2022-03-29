#include <APIC.h>
#include <Globals.h>
#include <cstr.h>
#include <PageFrameAllocator.h>
#include <Interrupts.h>

namespace APIC
{
	bool UsableAPIC = false;
	bool InUse = false;

	LocalAPIC* LAPIC = nullptr;
	LocalAPIC* PhysicalLAPIC = nullptr;

	uint16 IOAPICsCount = 0x00;
	IOAPIC IOAPICs[0x10];

	IRQRemapRecord* IRQOverrides[0xFF];
	uint16 IRQOverridesCount = 0x00;

	LAPICRecord* LocalAPICRecords[0xFF];
	uint16 LocalAPICRecordsCount = 0x00;

	byte IOAPICRecordsCount = 0x00;
	IOAPICRecord* IOAPICRecords[0xFF];


	void ParseMADT(MADTHeader* madt)
	{
		u64 MaxAddress = madt->Length + u64(madt);
		BaseRecord* currentRecord = (BaseRecord*)((u64)madt + sizeof(MADTHeader));
		u64 lApicPhysAddress = u64(madt->LapicAddress);

		IOAPICsCount = 0x00;
		IOAPICRecordsCount = 0x00;
		IRQOverridesCount = 0x00;
		LocalAPICRecordsCount = 0x00;

		while ((u64)currentRecord < MaxAddress)
		{
			switch (currentRecord->Type)
			{
				case RecordType::LAPIC:
				{
					LAPICRecord* lapic = (LAPICRecord*)currentRecord;
					LocalAPICRecords[LocalAPICRecordsCount++] = lapic;
					break;
				}

				case RecordType::IOAPIC:
				{
					IOAPICRecord* ioapic = (IOAPICRecord*)currentRecord;
					IOAPICRecords[IOAPICRecordsCount++] = ioapic;
					break;
				}

				case RecordType::IOAPICInterruptSourceOverride:
				{
					auto iso = (IRQRemapRecord*)currentRecord;
					IRQOverrides[iso->IrqSource] = iso;
					IRQOverridesCount++;
					debug("Found ISO: IrqSource: %x1, IrqOverride: %x1", iso->IrqOverride, iso->IrqSource);
					break;
				}

				case RecordType::LAPICNMI:
				{
					auto nmi = (ApicNmiRecord*)currentRecord;
					break;
				}

				case RecordType::LAPICAddressOverride:
				{
					ApicAddressOverrideRecord* addr = (ApicAddressOverrideRecord*)currentRecord;
					lApicPhysAddress = addr->Address;
					debug("LAPIC Address overidden: %x0", addr->Address);
					break;
				}

				case RecordType::x2LAPIC:
				{
					// idk wat 2 do with this lel
					UsableAPIC = false;
					//x2ApicCount++;
					return;
				}

			}
			currentRecord = (BaseRecord*)((u64)currentRecord + currentRecord->Length);
		}
		PhysicalLAPIC = (LocalAPIC*)lApicPhysAddress;
		UsableAPIC = true;
	}

	void Initialize(int cpu)
	{
		if (!UsableAPIC) return;
		InitializeLAPIC(cpu);
		InitializeIOAPIC();

		InUse = true;
	}

	void Enable()
	{
		if (!LAPIC) return;
		LAPIC->SpuriousInterruptVectorRegister = Interrupts::SpuriousInterruptVector | 0x100; // enable SI
	}

	void Disable()
	{
		if (!LAPIC) return;
		LAPIC->SpuriousInterruptVectorRegister = 0x00; // disble SI
	}

	void EndOfInterrupt()
	{
		if (LAPIC) LAPIC->EndOfInt.Set(0x00);
	}

}
