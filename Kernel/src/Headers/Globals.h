#pragma once
#ifndef H_Globals
#define H_Globals
#include <Types.h>
#include <Structs.h>
#include <PrimitiveConsole.h>
#include <GDT.h>

global PrimitiveConsole gConsole;
global GDT GlobalGDT; // yeah i know, "Global Global Descriptor Table" makes no sense. Cope.

// The value doesn't matter, we just need the address that they are at
global byte _KernelStart;
global byte _KernelEnd;

#endif