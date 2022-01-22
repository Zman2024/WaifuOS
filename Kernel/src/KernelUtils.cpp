#include <KernelUtils.h>

namespace Kernel
{
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