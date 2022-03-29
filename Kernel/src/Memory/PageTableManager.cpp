#include <PageTableManager.h>
#include <PageMapIndexer.h>
#include <PageFrameAllocator.h>
#include <MemoryUtils.hpp>

namespace PageTableManager
{
	PageTable* PML4;

	void PageTableManager(PageTable* pml4)
	{
		PML4 = pml4;
	}

	void MapMemory(void* virtualMemory, void* physMemory)
	{
		MapMemory((u64)virtualMemory, (u64)physMemory);
	}

	void SetVirtualFlag(nint address, PTFlag flag, bool value)
	{
		PageMapIndexer indexer = PageMapIndexer(address);
		PageDirectoryEntry tempPDE;

		tempPDE = PML4->Entries[indexer.PDPi];
		PageTable* PDP;
		{
			if (!tempPDE.GetFlag(PTFlag::Present))
			{
				PDP = PageFrameAllocator::RequestPage<PageTable>();
				memset<u64>(PDP, 0x00, PAGE_SIZE);

				tempPDE.SetAddress((u64)PDP >> 12);
				tempPDE.SetFlag(PTFlag::Present, true);
				tempPDE.SetFlag(PTFlag::ReadWrite, true);

				PML4->Entries[indexer.PDPi] = tempPDE;

			}
			else PDP = (PageTable*)(tempPDE.GetAddress() << 12);
		}

		tempPDE = PDP->Entries[indexer.PDi];
		PageTable* PD;
		{
			if (!tempPDE.GetFlag(PTFlag::Present))
			{
				PD = PageFrameAllocator::RequestPage<PageTable>();
				memset<u64>(PD, 0x00, PAGE_SIZE);

				tempPDE.SetAddress((u64)PD >> 12);
				tempPDE.SetFlag(PTFlag::Present, true);
				tempPDE.SetFlag(PTFlag::ReadWrite, true);

				PDP->Entries[indexer.PDi] = tempPDE;

			}
			else PD = (PageTable*)(tempPDE.GetAddress() << 12);
		}

		tempPDE = PD->Entries[indexer.PTi];
		PageTable* PT;
		{
			if (!tempPDE.GetFlag(PTFlag::Present))
			{
				PT = PageFrameAllocator::RequestPage<PageTable>();
				memset<u64>(PT, 0x00, PAGE_SIZE);

				tempPDE.SetAddress((u64)PT >> 12);
				tempPDE.SetFlag(PTFlag::Present, true);
				tempPDE.SetFlag(PTFlag::ReadWrite, true);

				PD->Entries[indexer.PTi] = tempPDE;

			}
			else PT = (PageTable*)(tempPDE.GetAddress() << 12);
		}


		tempPDE = PT->Entries[indexer.Pi];
		{
			tempPDE.SetFlag(flag, value);
		}

		PT->Entries[indexer.Pi] = tempPDE;
	}

	void MapMemory(u64 virtualMemory, u64 physMemory)
	{
		PageMapIndexer indexer = PageMapIndexer(virtualMemory);
		PageDirectoryEntry tempPDE;

		tempPDE = PML4->Entries[indexer.PDPi];
		PageTable* PDP;
		{
			if (!tempPDE.GetFlag(PTFlag::Present))
			{
				PDP = PageFrameAllocator::RequestPage<PageTable>();
				memset<u64>(PDP, 0x00, PAGE_SIZE);

				tempPDE.SetAddress((u64)PDP >> 12);
				tempPDE.SetFlag(PTFlag::Present, true);
				tempPDE.SetFlag(PTFlag::ReadWrite, true);

				PML4->Entries[indexer.PDPi] = tempPDE;

			}
			else PDP = (PageTable*)(tempPDE.GetAddress() << 12);
		}

		tempPDE = PDP->Entries[indexer.PDi];
		PageTable* PD;
		{
			if (!tempPDE.GetFlag(PTFlag::Present))
			{
				PD = PageFrameAllocator::RequestPage<PageTable>();
				memset<u64>(PD, 0x00, PAGE_SIZE);

				tempPDE.SetAddress((u64)PD >> 12);
				tempPDE.SetFlag(PTFlag::Present, true);
				tempPDE.SetFlag(PTFlag::ReadWrite, true);

				PDP->Entries[indexer.PDi] = tempPDE;

			}
			else PD = (PageTable*)(tempPDE.GetAddress() << 12);
		}

		tempPDE = PD->Entries[indexer.PTi];
		PageTable* PT;
		{
			if (!tempPDE.GetFlag(PTFlag::Present))
			{
				PT = PageFrameAllocator::RequestPage<PageTable>();
				memset<u64>(PT, 0x00, PAGE_SIZE);

				tempPDE.SetAddress((u64)PT >> 12);
				tempPDE.SetFlag(PTFlag::Present, true);
				tempPDE.SetFlag(PTFlag::ReadWrite, true);

				PD->Entries[indexer.PTi] = tempPDE;

			}
			else PT = (PageTable*)(tempPDE.GetAddress() << 12);
		}


		tempPDE = PT->Entries[indexer.Pi];
		{
			tempPDE.SetAddress(physMemory >> 12);
			tempPDE.SetFlag(PTFlag::Present, true);
			tempPDE.SetFlag(PTFlag::ReadWrite, true);
		}

		PT->Entries[indexer.Pi] = tempPDE;
	}

}