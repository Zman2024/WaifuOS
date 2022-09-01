#include <APIC.h>
#include <Globals.h>
#include <cstr.h>
#include <PageFrameAllocator.h>
#include <Interrupts.h>
#include <APICShared.h>

namespace APIC
{

	void ParseMADT(MADTHeader* madt)
	{
		debug("APIC Table: ");

		static bool _init = true;
		if (_init)
		{
			LocalAPICRecords = List<LAPICRecord*>();
			IOAPICs = List<IOAPIC*>();
			IOAPICRecords = List<IOAPICRecord*>();
			IRQOverrides = List<IRQRemapRecord*>(0xFF);
			_init = false;
		}

		u64 maxAddress = madt->Length + u64(madt);
		BaseRecord* currentRecord = (BaseRecord*)(u64(madt) + sizeof(MADTHeader));
		u64 lApicPhysAddress = u64(madt->LapicAddress);

		while ((u64)currentRecord < maxAddress)
		{
			switch (currentRecord->Type)
			{
				case RecordType::LAPIC:
				{
					LAPICRecord* lapic = (LAPICRecord*)currentRecord;
					LocalAPICRecords.Add(lapic);
					break;
				}

				case RecordType::IOAPIC:
				{
					IOAPICRecord* ioapic = (IOAPICRecord*)currentRecord;
					IOAPICRecords.Add(ioapic);
					break;
				}

				case RecordType::IOAPICInterruptSourceOverride:
				{
					auto iso = (IRQRemapRecord*)currentRecord;
					IRQOverrides[iso->IrqSource] = iso;
					debug("\tFound ISO: IrqSource: %x1, IrqOverride: %x1", iso->IrqOverride, iso->IrqSource);
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
					debug("\tLAPIC Address overidden: %x0", addr->Address);
					break;
				}

				case RecordType::x2LAPIC:
				{
					UsableAPIC = false;
					return;
				}

			}
			currentRecord = (BaseRecord*)((u64)currentRecord + currentRecord->Length);
		}

		LAPIC = (LocalAPIC*)lApicPhysAddress;
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
		if (!UsableAPIC) return;
		LAPIC->SpuriousInterruptVectorRegister = Interrupts::SpuriousInterruptVector | 0x100; // enable SI
	}

	void Disable()
	{
		if (!UsableAPIC) return;
		LAPIC->SpuriousInterruptVectorRegister = 0x00; // disble SI
	}

	void EndOfInterrupt()
	{
		if (InUse) LAPIC->EndOfInt.Set(0x00);
	}

}
