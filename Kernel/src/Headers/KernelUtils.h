#pragma once
#include <Types.h>
#include <Structs.h>
#include <EFI.h>
#include <PageFrameAllocator.h>
#include <Globals.h>

namespace Kernel
{
	void ShowLoadingImage(BootInfo* info);
	void InitializeKernel(BootInfo* bootInfo);
	global void KernelStart(BootInfo bootInfo);
}
