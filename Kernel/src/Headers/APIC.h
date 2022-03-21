#pragma once
#ifndef H_APIC
#define H_APIC
#include <ACPI.h>
#include <APICStructs.h>
#include <LocalAPIC.h>
#include <IOAPIC.h>

namespace APIC
{
	void ParseMADT(MADTHeader* madt);

	void Initialize(int cpu = 0);

	void Enable();
	void Disable();

	void EndOfInterrupt();
}

#endif