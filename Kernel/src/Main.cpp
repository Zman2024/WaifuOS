#include <KernelUtils.h>
#include <IOAPIC.h>

namespace Kernel
{
	inline void ClearBss(BootInfo bootInfo)
	{
		vptr bssStart = &_BssDataStart;
		vptr bssEnd = &_BssDataEnd;
		u64 bssSize = u64(bssEnd) - u64(bssStart);
		memset<u64>(bssStart, 0x00, bssSize);
	}

	global void KernelStart(BootInfo bootInfo)
	{
		// Clear uninitialized data (just to be sure)
		ClearBss(bootInfo);

		// Create console
		gConsole = PrimitiveConsole(bootInfo.Framebuffer, bootInfo.font);
		gConsole.Clear();

		// Show loading image
		ShowLoadingImage(&bootInfo);

		// Initialize Hardware
		Kernel::InitializeKernel(&bootInfo);

		gConsole.Write(OSName, Color::Green); gConsole.WriteLine(" Initialized!", Color::Green);
		while (true) hlt;
	}
}