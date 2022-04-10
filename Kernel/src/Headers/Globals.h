#pragma once
#ifndef H_Globals
#define H_Globals
#include <Types.h>
#include <Structs.h>
#include <PrimitiveConsole.h>
#include <GDT.h>
#include <IDT.h>
#include <Memory.h>
#include <string.h>

// #define NO_ANIME

global PrimitiveConsole gConsole;
global IDTR GlobalIDTR;
global GDT GlobalGDT; // yeah i know, "Global Global Descriptor Table" makes no sense. Cope.

// The value doesn't matter, we just need the address that they are at
global byte _KernelStart;
global byte _KernelEnd;

global byte _BssDataStart;
global byte _BssDataEnd;

#ifndef NO_ANIME
static char OSName[] = "WaifuOS";
#else
static char OSName[] = "OS";
#endif

extern byte GlobalIDTROffset[];

global byte _DataStart;
global byte _DataEnd;

static void debug(const char* str, int64 p0 = 0, int64 p1 = 0, int64 p2 = 0, int64 p3 = 0, int64 p4 = 0, int64 p5 = 0, int64 p6 = 0, int64 p7 = 0, int64 p8 = 0, int64 p9 = 0)
{
	gConsole.WriteLine(cstr::format(str, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9), Color32(0xB0B0B0));
}

static void warn(const char* str, int64 p0 = 0, int64 p1 = 0, int64 p2 = 0, int64 p3 = 0, int64 p4 = 0, int64 p5 = 0, int64 p6 = 0, int64 p7 = 0, int64 p8 = 0, int64 p9 = 0)
{
	gConsole.WriteLine(cstr::format(str, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9), Color::Yellow);
}

static void error(const char* str, int64 p0 = 0, int64 p1 = 0, int64 p2 = 0, int64 p3 = 0, int64 p4 = 0, int64 p5 = 0, int64 p6 = 0, int64 p7 = 0, int64 p8 = 0, int64 p9 = 0)
{
	gConsole.WriteLine(cstr::format(str, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9), Color::Red);
}

#endif