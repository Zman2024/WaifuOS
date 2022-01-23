#pragma once
#ifndef H_GDT
#define H_GDT
#include <Types.h>

struct GDTDescriptor
{
	uint16 Size;
	uint64 Offset;
} attribute((packed));

struct GDTEntry
{
	uint16 Limit0;
	uint16 Base0;

	byte Base1;
	byte AccessByte;
	byte Limit1Flags;
	byte Base2;
} attribute((packed));

struct GDT
{
	GDTEntry KernelNull;
	GDTEntry KernelCode;
	GDTEntry KernelData;

	GDTEntry UserNull;
	GDTEntry UserCode;
	GDTEntry UserData;

} attribute((packed)) attribute((aligned(PAGE_SIZE)));

global void LoadGDT(GDTDescriptor* desc);

#endif