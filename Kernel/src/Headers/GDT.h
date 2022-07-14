#pragma once
#ifndef H_GDT
#define H_GDT
#include <Types.h>

struct GDTDescriptor
{
	uint16 Size;
	uint64 Offset;
} attribute((packed));

// https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
struct AccessByte
{
	inline AccessByte(byte value) { *(byte*)this = value; }

	byte Accessed : 1;
	byte ReadWrite : 1;
	byte Direction : 1;
	byte Executable : 1;
	byte DescriptorType : 1; // must be 1 for code / data segment descriptor
	byte PrivilegeLevel : 2;
	byte Present : 1;
	
} attribute((packed));

// https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
struct SegmentDescriptor
{
	uint16 Limit0;
	uint16 Base0;

	byte Base1;
	AccessByte Access;
	byte Limit1Flags;
	byte Base2;

	inline void SetBase(uint32 base)
	{
		Base0 = base & 0xFFFF;

		base >>= 16;
		Base1 = base & 0xFF;

		base >>= 8;
		Base2 = base & 0xFF;
	}

} attribute((packed));

// https://wiki.osdev.org/Global_Descriptor_Table#System_Segment_Descriptor 
struct SystemAccessByte
{
	inline SystemAccessByte(byte value) { *(byte*)this = value; }

	byte SystemDescriptorType : 4;
	byte DescriptorType : 1;	// must be 0 for System Segment Descriptor
	byte PrivilegeLevel : 2;
	byte Present : 1;

} attribute((packed));

// https://wiki.osdev.org/Global_Descriptor_Table#Long_Mode_System_Segment_Descriptor
struct SystemSegmentDescriptor
{
	uint16 Limit0;
	uint16 Base0;

	byte Base1;
	SystemAccessByte Access;
	byte Limit1Flags;
	byte Base2;

	uint32 Base3;
	uint32 rsv0;

	forceinline void SetBase(vptr base)
	{
		SetBase((u64)base);
	}

	inline void SetBase(uint64 base)
	{
		Base0 = base & 0xFFFF;

		base >>= 16;
		Base1 = base & 0xFF;

		base >>= 8;
		Base2 = base & 0xFF;

		base >>= 8;
		Base3 = base & 0xFFFFFFFF;
	}

	inline void SetLimit(uint32 limit)
	{
		limit &= 0xFFFFF;
		Limit0 = limit & 0xFFFF;
		limit >>= 16;

		// extract top 4 bits (flags)
		byte flags = Limit1Flags & 0xF0;

		Limit1Flags = limit & 0xF;
		Limit1Flags |= flags; // restore flags
	}

} attribute((packed));

constexpr uint16 KERNEL_CODE_GDT_INDEX = 0x1;
constexpr uint16 KERNEL_DATA_GDT_INDEX = 0x2;
constexpr uint16 USER_CODE_GDT_INDEX = 0x3;
constexpr uint16 USER_DATA_GDT_INDEX = 0x4;
constexpr uint16 TSS_GDT_INDEX = 0x5;

struct GDT
{
	SegmentDescriptor KernelNull;
	SegmentDescriptor KernelCode;
	SegmentDescriptor KernelData;

	SegmentDescriptor UserCode;
	SegmentDescriptor UserData;
	SystemSegmentDescriptor TaskSS;

} attribute((packed)) attribute((aligned(0x10)));

global void LoadGDT(GDTDescriptor* desc);

#endif