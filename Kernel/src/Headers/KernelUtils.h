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
#include <ACPI.h>
#include <PCI.h>
#include <APIC.h>
#include <IOAPIC.h>
#include <CPUIDCodes.h>
#include <PIC.hpp>
#include <IOBus.hpp>
#include <string.h>
#include <PIT.h>
#include <RTC.h>

namespace Kernel
{
	// entry0 for the kernel
	global void KernelStart(BootInfo bootInfo);

	// Initializes hardware and kernel resources
	void InitializeKernel(const BootInfo& bootInfo);

	// Use PIC / APIC
	void InitializeIRQ();

	// Init ACPI
	void InitializeACPI(const BootInfo& bootInfo);

	// Initializes Paging and Memory Mapping
	void InitializePaging(const BootInfo& bootInfo);

	// Initializes interrupts (duh)
	void InitializeExceptions();

	// Initializes the global descriptor table for interrupts
	void InitializeGDT();

	// Shows loading image loaded from bootloader
	void ShowLoadingImage(const BootInfo& info);
}

#endif