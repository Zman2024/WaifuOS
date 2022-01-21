#ifndef H_Funcs
#define H_Funcs
#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "Structs.h"

UINTN strcmp(CHAR8* str1, CHAR8* str2, UINTN length)
{
	for (UINTN x = 0; x < length; x++)
	{
		if (*str1 != *str2) return 0;
		str1++;
		str2++;
	}
	return 1;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_FILE* LoadedFile;
	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	EFI_STATUS Status;

	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL)
	{
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	Status = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

	if (Status != EFI_SUCCESS)
	{
		return NULL;
	}

	return LoadedFile;
}

int memcmp(const void* ptr1, const void* ptr2, size_t size)
{
	const byte* cst1 = ptr1, *cst2 = ptr2;

	for (size_t x = 0; x < size; x++)
	{
		if (cst1[x] != cst2[x])
			return -1;
	}

	return 0;
}

PSF1Font* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_FILE* font = LoadFile(Directory, Path, ImageHandle, SystemTable);
	if (font == NULL) return NULL;

	PSF1Header* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1Header), (void**)&fontHeader);
	UINTN size = sizeof(PSF1Header);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF_MAGIC0 || fontHeader->magic[1] != PSF_MAGIC1)
	{
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;

	if (fontHeader->mode == 1)
	{
		//512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1Header));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1Font* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1Font), (void**)&finishedFont);

	finishedFont->Header = fontHeader;
	finishedFont->GlyphBuffer = glyphBuffer;

	return finishedFont;

}

Framebuffer fb;
Framebuffer* InitializeGOP()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

	if (EFI_ERROR(status))
	{
		Print(L"Unable to locate GOP!\n\r");
		return NULL;
	}

	Print(L"GOP Located\n\r");

	fb.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	fb.BufferSize = gop->Mode->FrameBufferSize;
	fb.Width = gop->Mode->Info->HorizontalResolution;
	fb.Height = gop->Mode->Info->VerticalResolution;
	fb.PixelsPerScanline = gop->Mode->Info->PixelsPerScanLine;

	return &fb;
}

#endif