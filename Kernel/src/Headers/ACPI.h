#pragma once
#ifndef H_ACPI
#define H_ACPI
#include <Types.h>
#include <cstr.h>

namespace ACPI
{
	struct RSDP2
	{
		byte Signature[8];
		byte Checksum;
		byte OEMID[6];
		byte Revision;
		uint32 RSDTAddress;
		uint32 Length;
		uint64 XSDTAddress;
		byte ExtendedChecksum;
		byte Reserved[3];
	} attribute((packed));

	struct XSDTHeader
	{
		byte Signature[4];
		uint32 Length;
		byte Revision;
		byte Checksum;
		byte OEMID[6];
		byte OEMTableID[8];
		uint32 OEMRevision;
		uint32 CreatorID;
		uint32 CreatorRevision;
	} attribute((packed));

	struct MCFGHeader : XSDTHeader
	{
	private:
		uint64 Reserved;
	} attribute((packed));

	void* FindTable(XSDTHeader* sdtHeader, const char* signature);

}

#endif
