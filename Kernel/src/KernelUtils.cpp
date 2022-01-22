#include <KernelUtils.h>
#include <Globals.h>

namespace Kernel
{
	void InitializeKernel(BootInfo* bootInfo)
	{
		gConsole = PrimitiveConsole(bootInfo->Framebuffer, bootInfo->font);
		// gConsole.Clear();

	}
}