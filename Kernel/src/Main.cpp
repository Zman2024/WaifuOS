#include <KernelUtils.h>

namespace Kernel
{
	forceinline void ClearBss(BootInfo& bootInfo)
	{
		vptr bssStart = &_BssDataStart;
		vptr bssEnd = &_BssDataEnd;
		u64 bssSize = u64(bssEnd) - u64(bssStart);
		memset64(bssStart, 0x00, bssSize);
	}

	global void KernelStart(BootInfo bootInfo)
	{
		// Clear uninitialized data (just to be sure)
		ClearBss(bootInfo);

		// Create console
		gConsole = PrimitiveConsole(bootInfo.Framebuffer, bootInfo.font);
		gConsole.Clear();

		#ifndef NO_ANIME
		// Show loading image
		ShowLoadingImage(bootInfo);
		#endif

		// Initialize Hardware //
		Kernel::InitializeKernel(bootInfo);

		gConsole.WriteLine(string(OSName) + " Initialized!", Color::Green);
		Memory::PrintLeaks();

		gConsole.EnableCursor();

		while (true) hlt;
	}
}