#include <ACPI.h>
#include <string.h>
#include <Globals.h>

namespace ACPI
{

	void PrintTables(XSDTHeader* xsdt)
	{
		u32 entries = (xsdt->Length - sizeof(XSDTHeader)) / 8;
		debug("Available ACPI tables:");
		for (u32 x = 0; x < entries; x++)
		{
			XSDTHeader* head = (XSDTHeader*)*(u64*)((u64)xsdt + sizeof(XSDTHeader) + (x * 8));
			debug("\t" + string(head->Signature, sizeof(head->Signature)));
		}
	}
	
	void* FindTable(XSDTHeader* xsdt, const char* signature)
	{
		u32 entries = (xsdt->Length - sizeof(XSDTHeader)) / 8;

		for (u32 x = 0; x < entries; x++)
		{
			XSDTHeader* head = (XSDTHeader*)*(u64*)((u64)xsdt + sizeof(XSDTHeader) + (x * 8));
			if (cstr::strcmp((char*)head->Signature, signature, 4)) return head;
		}

		return nullptr;
	}

}
