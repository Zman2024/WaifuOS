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

	void InitializeKernel(BootInfo* bootInfo)
	{
		// Create console
		gConsole = PrimitiveConsole(bootInfo->Framebuffer, bootInfo->font);

		// Read EFI memory map and set up PFA
		PageFrameAllocator::ReadEfiMemoryMap(bootInfo->MemoryMap, bootInfo->MapSize, bootInfo->MapDescriptorSize);

		// Lock the kernel
		u64 kSize = (u64)&_KernelEnd - (u64)&_KernelStart;
		PageFrameAllocator::LockPages(&_KernelStart, kSize / PAGE_SIZE + 1);

		// Lock the framebuffer
		PageFrameAllocator::LockPages(bootInfo->Framebuffer->BaseAddress, (bootInfo->Framebuffer->BufferSize + PAGE_SIZE) / PAGE_SIZE);

	}
}