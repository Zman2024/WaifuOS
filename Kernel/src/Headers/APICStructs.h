#pragma once
#ifndef H_APICStructs
#define H_APICStructs
#include <ACPI.h>
#include <Types.h>
#include <Globals.h>

// Contains the structs that the LocalAPIC / IOAPIC use
// APIC code can be found in APIC.h and APIC.cpp
namespace APIC
{
	// much helpful: https://wiki.osdev.org/MADT https://wiki.osdev.org/APIC https://wiki.osdev.org/IOAPIC 

	namespace TriggerMode
	{
		enum : byte
		{
			EDGE = 0,
			LEVEL = 1,
		};
	};

	namespace DeliveryMode
	{
		enum : byte
		{
			Fixed = 0b000, // edge or level
			LowestPriority = 0b001, // edge or level
			SystemManagemnet = 0b010, // SMI, System Management Interrupt (requires edge)
			Reserved0 = 0b011, // reserved
			NonMaskable = 0b100, // treated as edge, so program it as that
			Initialize = 0b101, // Tells APIC to enter Init state (requires edge)
			Reserved1 = 0b110, // reserved
			External = 0b111, // edge required
		};
	}

	namespace Polarity
	{
		enum : byte
		{
			ActiveHIGH = 0x00,
			ActiveLOW = 0x01,
		};
	}

	namespace DestinationMode
	{
		enum : byte
		{
			PHYSICAL = 0,
			LOGICAL = 1,
		};
	}

	namespace ioapicRegister
	{
		enum : uint32
		{
			IOAPIC_ID = 0x00,
			Version = 0x01,
			ArbitrationPriority = 0x02,
		};
	}

	// https://pdos.csail.mit.edu/6.828/2014/readings/ia32/ioapic.pdf page 11 - 13
	struct RedirectionEntry
	{
		inline RedirectionEntry() {};
		inline RedirectionEntry(u64 value) { *(u64*)this = value; }

		uint64 InterruptVector : 8;
		uint64 DeliveryMode : 3;
		uint64 DestinationMode : 1; // 0 = PHYSICAL, 1 = LOGICAL
		uint64 ApicBusy : 1;	// 0 = IDLE, 1 = PENDING (delivery pending)
		uint64 Polarity : 1;
		uint64 LevelRecieved : 1; // This bit is set to 1 when local APIC(s) accept the level interrupt sent by the IOAPIC
		uint64 TriggerMode : 1;
		uint64 Mask : 1; // true = disabled, false = enabled.
		uint64 rsv0 : 39;
		uint64 DestinationField : 8;

		inline operator u64() { return *(u64*)this; }
	};

	namespace RecordType
	{
		enum : byte
		{
			LAPIC = 0x00,
			IOAPIC = 0x01,
			IOAPICInterruptSourceOverride = 0x02,
			IOAPICNMISource = 0x03,
			LAPICNMI = 0x04,
			LAPICAddressOverride = 0x05,
			x2LAPIC = 0x09,
		};
	}

	struct MADTHeader : ACPI::XSDTHeader
	{
		uint32 LapicAddress;
		uint32 Flags;

	} attribute((packed));

	struct BaseRecord
	{
		byte Type;
		byte Length;
	};

	struct LAPICRecord : BaseRecord
	{
		byte AcpiProcessorID;
		byte ApicID;

		/*
			bit 0: Processor Enabled (set if ready for use)
			bit 1: Online Capable (indicates if the APIC can be enabled at runtime)
		*/
		uint32 Flags;
	};

	struct IOAPICRecord : BaseRecord
	{
		byte ioApicID;
		byte rsv0;

		uint32 Address;
		uint32 GlobalSysInterruptBase;
	};

	struct IRQRemapRecord : BaseRecord
	{
		byte BusSource;
		byte IrqSource; // the IRQ number that is being replaced
		uint32 IrqOverride; // the IRQ number that is replacing the original
		uint16 Flags;
	};

	struct ApicNmiRecord : BaseRecord
	{
		byte AcpiProcessorID;
		uint16 Flags;
		byte LInt;
	};

	struct ApicAddressOverrideRecord : BaseRecord
	{
		uint64 Address;
	};

	struct x2APICRecord
	{
		uint16 rsv0; // Should be zero


		uint32 x2ApicID; // x2LocalAPIC processor ID

		/*
			bit 0: Processor Enabled (set if ready for use)
			bit 1: Online Capable (indicates if the APIC can be enabled at runtime)
		*/
		uint32 Flags;
		uint32 ApicID;
	};

	struct Register
	{
	public:
		volatile uint32 value;

		inline uint32 Get()
		{
			uint32 volatile* ptr = &this->value;
			return *ptr;
		}

		inline void Set(uint32 val)
		{
			uint32 volatile* ptr = &this->value;
			*ptr = val;
		}

		inline operator uint32() { return this->Get(); }
		inline void operator =(uint32 value) { this->Set(value); }
	private:
		volatile byte pad[0x10 - 4];
	}; // might need attribute((packed)); also i could just align((0x10))

	struct RegisterCluster
	{
		Register R0;
		Register R1;
		Register R2;
		Register R3;
		Register R4;
		Register R5;
		Register R6;
		Register R7;
	};

	// I hate this
	struct LocalAPIC
	{
		byte Reserved0[0x20];

		Register LapicIDRegister;

		Register LapicVersionRegister;

		byte Reserved1[0x40];

		Register TaskPriorityRegister;

		Register ArbitrationPriorityRegister;

		Register ProcessorPriorityRegister;

		Register EndOfInt;

		Register RemoteReadRegister;

		Register LogicalDestinationRegister;

		Register DestinationFormatRegister;

		Register SpuriousInterruptVectorRegister;

		RegisterCluster InServiceRegister;

		RegisterCluster TriggerModeRegister;

		RegisterCluster InterruptRequestRegister;

		Register ErrorStatusRegister;

		byte Reserved2[0x60];

		Register LVTCorrectedMachineCheckInterruptRegister;

		Register InterruptCommandRegister0;

		Register InterruptCommandRegister1;

		Register LVTTimerRegister;

		Register LVTThermalSensorRegister;

		Register LVTPerformanceMonitoringCountersRegister;

		Register LVTLINT0Register;

		Register LVTLINT1Register;

		Register LVTErrorRegister;

		Register InitialCountRegister;

		Register CurrentCountRegister;

		byte Reserved3[0x40];

		Register DivideConfigurationRegister;

		byte Reserved4[0x10];
	};

	// Much help: https://wiki.osdev.org/IOAPIC 
	struct IOAPIC
	{
		IOAPIC();
		IOAPIC(IOAPICRecord* ioapic);

		uint32 ReadIOAPIC(uint32 reg);
		void WriteIOAPIC(uint32 reg, uint32 value);

		RedirectionEntry ReadIOAPICRedirectionEntry(byte entry);
		void WriteIOAPICRedirectionEntry(byte entry, RedirectionEntry val);

		inline byte GetID() { return this->mIOApicId; }
		inline byte GetVersion() { return this->mVersion; }
		inline byte GetMaxRedirectionEntries() { return this->mMaxRedEntryCount; }
		inline byte GetCurrentRedirectionEntries() { return this->mCurrentRedEntryCount; }
		inline uint64 GetInterruptBase() { return this->mGlobalInterruptBase; }

	protected:
		static const u32 IOApicRedirectionTableOffset = 0x10;

		vptr mPhysicalAddress;
		vptr mVirtualAddress;

		uint32 volatile* mRegIORW;
		uint32 volatile* mRegSel;

		IOAPICRecord mRecord;

		byte mIOApicId;
		byte mVersion;

		byte mMaxRedEntryCount;
		byte mCurrentRedEntryCount;

		// The first IRQ this ioapic handles, found in MADT 2.0
		uint64 mGlobalInterruptBase;

	};

	// Local APIC ptr
	extern LocalAPIC* LAPIC;
	// Physical address of the LAPIC
	extern LocalAPIC* PhysicalLAPIC;

	extern uint16 IOAPICsCount;
	extern IOAPIC IOAPICs[];

	// index = IRQ num that's been overidden
	// if IRQOverrides[irq] == nullptr then there isn't a source override
	extern IRQRemapRecord* IRQOverrides[];
	extern uint16 IRQOverridesCount;

	extern LAPICRecord* LocalAPICRecords[];
	extern uint16 LocalAPICRecordsCount;

	extern byte IOAPICRecordsCount;
	extern IOAPICRecord* IOAPICRecords[];

	// When true, the APIC is found and usable
	extern bool UsableAPIC;
	// When true, the apic is in use
	extern bool InUse;

}

#endif