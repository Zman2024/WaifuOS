#include <Types.h>
#include <Structs.h>
#include <KernelUtils.h>
#include <Globals.h>
#include <cstr.h>
#include <Memory.h>
#include <PageFrameAllocator.h>

namespace Kernel
{
	void ShowLoadingImage(BootInfo* info)
	{
		Color* colorFB = (Color*)info->Framebuffer->BaseAddress;
		Color* img = (Color*)(info->LoadingImage);

		for (uint32 y = 0; y < 256; y++)
		{
			for (uint32 x = 0; x < 256; x++)
			{
				colorFB[((y + (uint64)(info->Framebuffer->Height/2)-128) * info->Framebuffer->PixelsPerScanline) + x + (info->Framebuffer->PixelsPerScanline/2) - 128] = img[(y * 256) + x];
			}
		}

	}

	global void KernelStart(BootInfo bootInfo)
	{
		Kernel::InitializeKernel(&bootInfo);
		gConsole.Clear();
		gConsole.WriteLine("Hello WaifuOS!");
		

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