#include <IOAPIC.h>
#include <APIC.h>
#include <PageFrameAllocator.h>
#include <PageTableManager.h>
#include <MemoryUtils.hpp>

namespace APIC
{
	IOAPIC::IOAPIC()
	{
		memset<u64>((vptr)this, 0x00, sizeof(IOAPIC));
	}

	IOAPIC::IOAPIC(IOAPICRecord* ioapic)
	{
		// address shit first
		this->mPhysicalAddress = this->mVirtualAddress = vptr((u64)ioapic->Address);

		// No reason to memory map it somewhere else
		//if (!this->mVirtualAddress)
		//{
		//	debug("IOAPIC: PFA didn't give me a valid address (nullptr, very zad) we just memory map to physical");
		//	this->mVirtualAddress = this->mPhysicalAddress;
		//}

		PageTableManager::MapMemory(this->mVirtualAddress, this->mPhysicalAddress);

		// Lock physical page
		PageFrameAllocator::LockPage(this->mPhysicalAddress);

		mRegSel = (uint32 volatile*)mVirtualAddress;
		mRegIORW = (uint32 volatile*)(u64(mVirtualAddress) + 0x10);

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
		*mRegSel = (reg & 0xFF);
		return *mRegIORW;
	}

	void IOAPIC::WriteIOAPIC(uint32 reg, uint32 value)
	{
		*mRegSel = (reg & 0xFF);
		*mRegIORW = value;
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
