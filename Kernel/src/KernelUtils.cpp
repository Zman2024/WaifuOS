#include <KernelUtils.h>

namespace Kernel
{
	void ShowLoadingImage(BootInfo* info)
	{
		Color* colorFB = (Color*)info->Framebuffer->BaseAddress;
		Color* img = (Color*)(info->LoadingImage);

		for (u16 y = 0; y < 256; y++)
		{
			for (u16 x = 0; x < 256; x++)
			{
				// wow this is massive, cringe. 10/10 very readable code
				colorFB[((y + (u64)(info->Framebuffer->Height / 1.5) - 128) * info->Framebuffer->PixelsPerScanline) + x + (info->Framebuffer->PixelsPerScanline / 2) - 128] = img[(y * 256) + x];
			}
		}

	}

	void InitializeGDT()
	{
		// Create and load GDT
		GDTDescriptor desc{};
		desc.Size = sizeof(GDT) - 1;
		desc.Offset = (u64)&GlobalGDT;
		LoadGDT(&desc);
	}

	void InitializeKernel(BootInfo* bootInfo)
	{
		cli;

		// Create console
		gConsole = PrimitiveConsole(bootInfo->Framebuffer, bootInfo->font);
		
		// Do GDT stuff
		InitializeGDT();

		// Paging / Memory Mapping
		InitializePaging(bootInfo);
		gConsole.Clear();

		if (bootInfo->LoadingImage) ShowLoadingImage(bootInfo);
		else gConsole.WriteLine("ERROR: NO BOOT IMAGE", Color::Red);

		sti;
	}

	void InitializePaging(BootInfo* bootInfo)
	{
		u64 nMapEntries = bootInfo->MapSize / bootInfo->MapDescriptorSize;

		// Read EFI memory map and set up PFA
		PageFrameAllocator::ReadEfiMemoryMap(bootInfo->MemoryMap, bootInfo->MapSize, bootInfo->MapDescriptorSize);

		// Lock the framebuffer
		u64 fbBase = (u64)bootInfo->Framebuffer->BaseAddress;
		u64 fbSize = (u64)bootInfo->Framebuffer->BufferSize + PAGE_SIZE;
		PageFrameAllocator::LockPages((void*)fbBase, fbSize / PAGE_SIZE);

		// Lock the kernel
		u64 kSize = (u64)&_KernelEnd - (u64)&_KernelStart;
		PageFrameAllocator::LockPages(&_KernelStart, kSize / PAGE_SIZE + 1);

		// Initialize PageTableManager
		PageTable* PML4 = PageFrameAllocator::RequestPage<PageTable>();
		memset<u64>(PML4, 0x00, PAGE_SIZE);
		PageTableManager::PageTableManager(PML4);

		// Map all memory (this causes rainbow in qemu)
		u64 memorySize = Memory::CalculateMemorySize(bootInfo->MemoryMap, nMapEntries, bootInfo->MapDescriptorSize);
		for (u64 x = 0; x < memorySize; x += PAGE_SIZE)
		{
			PageTableManager::MapMemory(x, x);
		}

		// Remap Framebuffer
		for (u64 x = fbBase; x < fbBase + fbSize; x += PAGE_SIZE)
		{
			PageTableManager::MapMemory(x, x);
		}

		// Load PML4 into control3
		asm("mov %%cr3, %0" : : "r" (PML4));
	}

}