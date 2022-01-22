#include <Types.h>
#include <Structs.h>
#include <KernelUtils.h>
#include <Globals.h>
#include <cstr.h>
#include <Memory.h>
#include <PageFrameAllocator.h>

namespace Kernel
{
	global void KernelStart(BootInfo bootInfo)
	{
		Kernel::InitializeKernel(&bootInfo);
		gConsole.Clear();
		gConsole.WriteLine("Hello WaifuOS!");
		
		gConsole.WriteLine(cstr::ToString(cstr::ToInt("-100")));
		gConsole.WriteLine(cstr::ToString(cstr::ToInt("ABC", true), true));

		for (byte x = 0; x < 10; x++)
		{
			uint64 bruh = (u64)PageFrameAllocator::RequestPage<void>();

			gConsole.Write("Page Address: ");
			gConsole.WriteLine(cstr::ToString(bruh, true), Color::Cyan);
		}

		if (bootInfo.LoadingImage) ShowLoadingImage(&bootInfo);

		OS_HLT;
	}
}