#pragma once
#ifndef H_Structs
#define H_Structs
#include <Types.h>
#include <EFI.h>

struct FontHeader
{
	byte magic[2];
	byte mode;
	byte charsize;
};

struct Font
{
	FontHeader* Header;
	void* GlyphBuffer;
};

struct FrameBuffer
{
	void* BaseAddress;
	size_t BufferSize;
	uint32 Width;
	uint32 Height;
	uint32 PixelsPerScanline;
};

struct BootInfo
{
	FrameBuffer* Framebuffer;
	Font* font;
	EfiMemoryDescriptor* MemoryMap;
	uint64 MapSize;
	uint64 MapDescriptorSize;
	void* RSDP;
};

struct Rectangle
{
	uint64 Width;
	uint64 Height;

	Rectangle()
	{
		Width = 0;
		Height = 0;
	}

	Rectangle(uint64 x, uint64 y)
	{
		Width = x;
		Height = y;
	}
};

struct sPoint
{
	int64 X;
	int64 Y;

	sPoint()
	{
		X = 0;
		Y = 0;
	}

	sPoint(int64 x, int64 y)
	{
		X = x;
		Y = y;
	}
};

struct Version
{
	byte Major;
	byte Minor;
	byte Revision;
	byte Extra [4];
};
#endif