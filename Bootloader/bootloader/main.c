#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "Structs.h"
#include "Funcs.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	InitializeLib(ImageHandle, SystemTable);
	Print(L"Bootloader Initialized\n\r");

	EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

	uint64_t pos = 0;
	Kernel->GetPosition(Kernel, &pos);

	if (Kernel == NULL)
	{
		Print(L"Error reading kernel!\n\r");
		return EFI_LOAD_ERROR;
	}

	Print(L"Kernel read successfully\n\r");

	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		Kernel->Read(Kernel, &size, &header);
	}

	void* ImageData = LoadFileRaw(NULL, L"loadingicon.raw", ImageHandle, SystemTable);


	if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT)
	{
		Print(L"Kernel has wrong format!\n\r");
		return EFI_LOAD_ERROR;
	}

	Print(L"Kernel header successfully verified!\n\r");

	Elf64_Phdr* phdrs;
	{
		Kernel->SetPosition(Kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		Kernel->Read(Kernel, &size, phdrs);
	}

	for (Elf64_Phdr* phdr = phdrs; 
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize; 
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize) )
	{
		
		switch (phdr->p_type)
		{
			case PT_LOAD:	// PT_LOAD
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				// Zero it out for uninitialized data (real hardware issues, am i right?)
				for (size_t x = 0; x < phdr->p_memsz; x++)
				{
					((uint8_t*)segment)[x] = 0x00;
				}

				Kernel->SetPosition(Kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				Kernel->Read(Kernel, &size, (void*)segment);
				break;
			}
		}
	}

	Print(L"Kernel loaded!\n\r");

	PSF1Font* newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);
	if (newFont == NULL)
	{
		Print(L"Invalid font!\n\r");
		return -1;
	}
	Print(L"Font found and loaded\n\rchar size: %d\n\r", newFont->Header->charsize);

	Framebuffer* newBuffer = InitializeGOP();

	Print(L"Base: 0x%x\n\rSize: 0x%x\n\rWidth: %d\n\rHeight: %d\n\rPixelsPerScanline: %d\n\r", 

		newBuffer->BaseAddress,
		newBuffer->BufferSize,
		newBuffer->Width,
		newBuffer->Height,
		newBuffer->PixelsPerScanline);

	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	uint64_t MapSize, MapKey, DescriptorSize;
	uint32_t DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}

	EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	void* rsdp = NULL;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;

	for (UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++)
	{
		if (CompareGuid(&configTable[index].VendorGuid, &Acpi2TableGuid))
		{
			if (strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable, 8))
			{
				rsdp = (void*)configTable->VendorTable;
			}
		}
		configTable++;
	}


	void (*KernelStart)(BootInfo) = ((__attribute__((sysv_abi)) void(*)(BootInfo)) header.e_entry);

	BootInfo info;
	{
		info.Framebuffer = newBuffer;
		info.Font = newFont;
		info.MemoryMap = Map;
		info.MapSize = MapSize;
		info.DescriptorSize = DescriptorSize;
		info.RSDP = rsdp;
		info.LoadingImage = ImageData;
	}

	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
	KernelStart(info);
	return EFI_SUCCESS; // Exit the UEFI application
}
