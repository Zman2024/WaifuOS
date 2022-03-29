#pragma once
#ifndef H_PageTableManager
#define H_PageTableManager
#include <Types.h>
#include <Paging.h>

namespace PageTableManager
{
	void PageTableManager(PageTable* PML4);
	void MapMemory(void* virtualMemory, void* physMemory);
	void MapMemory(u64 virtualMemory, u64 physMemory);
	void SetVirtualFlag(nint address, PTFlag flag, bool value);
	inline void SetVirtualFlag(vptr address, PTFlag flag, bool value) { SetVirtualFlag((nint)address, flag, value); }
}

#endif