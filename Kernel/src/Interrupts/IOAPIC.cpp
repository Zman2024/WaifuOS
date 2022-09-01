#include <IOAPIC.h>
#include <APIC.h>
#include <PageFrameAllocator.h>
#include <PageTableManager.h>
#include <Memory.h>
#include <APICShared.h>

namespace APIC
{
	IOAPIC::IOAPIC()
	{
		memset64((vptr)this, 0x00, sizeof(IOAPIC));
	}

	IOAPIC::IOAPIC(IOAPICRecord* ioapic)
	{
		// address shit first
		this->mPhysicalAddress = vptr((u64)ioapic->Address);

		// Map to itself
		PageTableManager::MapMemory(this->mPhysicalAddress, this->mPhysicalAddress);

		// Setup PTFlags
		//PageTableManager::SetVirtualFlag(this->mPhysicalAddress, PTFlag::Present, true);
		//PageTableManager::SetVirtualFlag(this->mPhysicalAddress, PTFlag::ReadWrite, true);

		// Lock physical page
		PageFrameAllocator::LockPage(this->mPhysicalAddress);

		mRegisterSelect = (uint32 volatile*)this->mPhysicalAddress;
		mRegisterRW = (uint32 volatile*)(u64(this->mPhysicalAddress) + 0x10);

		this->mIOApicId = ioapic->ioApicID;
		this->mGlobalInterruptBase = ioapic->GlobalSysInterruptBase;
		this->mRecord = *ioapic;

		this->mVersion = byte(this->ReadIOAPIC(ioapicRegister::Version));
		this->mMaxRedEntryCount = byte(this->ReadIOAPIC(ioapicRegister::Version) >> 16) + 1;

		// mask everything
		for (byte x = mGlobalInterruptBase; x < mMaxRedEntryCount + mGlobalInterruptBase; x++)
		{
			RedirectionEntry entry = x;
			entry.Mask = true;
			WriteIOAPICRedirectionEntry(x, entry);
		}

		debug("Initialized IOAPIC with ID #%0 at address: %x1", this->mIOApicId, u64(this->mPhysicalAddress));
	}

	uint32 IOAPIC::ReadIOAPIC(uint32 reg)
	{
		*mRegisterSelect = (reg & 0xFF);
		return *mRegisterRW;
	}

	void IOAPIC::WriteIOAPIC(uint32 reg, uint32 value)
	{
		*mRegisterSelect = (reg & 0xFF);
		*mRegisterRW = value;
	}

	RedirectionEntry IOAPIC::ReadIOAPICRedirectionEntry(byte entry)
	{
		uint32 reg = (entry * 2) + IOApicRedirectionTableOffset;
		uint64 ret = u64(this->ReadIOAPIC(reg));
		ret |= u64(this->ReadIOAPIC(reg + 1)) << 32;
		return ret;
	}

	void IOAPIC::WriteIOAPICRedirectionEntry(byte entry, RedirectionEntry val)
	{
		entry -= this->mGlobalInterruptBase;
		if (entry >= this->mMaxRedEntryCount) return;

		uint64 value = val;
		uint32 reg = (entry * 2) + IOApicRedirectionTableOffset;

		this->WriteIOAPIC(reg, u32(value));
		this->WriteIOAPIC(reg + 1, u32(value >> 32));
	}

}
