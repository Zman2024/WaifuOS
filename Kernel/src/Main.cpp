#include <Types.h>
#include <Structs.h>
#include <KernelUtils.h>
#include <Globals.h>
#include <cstr.h>
#include <Memory.h>
#include <PageFrameAllocator.h>
#include <CPUIDCodes.h>

namespace Kernel
{
	global void KernelStart(BootInfo bootInfo)
	{
		Kernel::InitializeKernel(&bootInfo);
		gConsole.WriteLine("WaifuOS Initialized!", Color::Green);

		halt;
	}
}