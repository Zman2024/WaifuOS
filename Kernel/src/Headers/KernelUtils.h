#pragma once
#ifndef H_KernelUtils
#define H_KernelUtils
#include <Types.h>
#include <Structs.h>
#include <EFI.h>
#include <PageFrameAllocator.h>
#include <Globals.h>
#include <PageMapIndexer.h>
#include <cstr.h>
#include <Paging.h>
#include <MemoryUtils.hpp>
#include <PageTableManager.h>
#include <GDT.h>
#include <Interrupts.h>

namespace Kernel
{
	// entry0 for the kernel
	global void KernelStart(BootInfo bootInfo);

	// Initializes hardware and kernel resources
	void InitializeKernel(BootInfo* bootInfo);

	// Initializes Paging and Memory Mapping
	void InitializePaging(BootInfo* bootInfo);

	// Initializes interrupts (duh)
	void InitializeInterrupts();

	// Initializes the global descriptor table for interrupts
	void InitializeGDT();

	// Shows loading image loaded from bootloader
	void ShowLoadingImage(BootInfo* info);
}

#endif