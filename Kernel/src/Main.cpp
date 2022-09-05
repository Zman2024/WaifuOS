#include <KernelUtils.h>
#include <Synchronization.hpp>
#include <Scheduling.h>
#include <FAT32.h>

namespace Kernel
{
	forceinline void ClearBss(BootInfo& bootInfo)
	{
		vptr bssStart = &_BssDataStart;
		vptr bssEnd = &_BssDataEnd;
		u64 bssSize = u64(bssEnd) - u64(bssStart);
		memset64(bssStart, 0x00, bssSize);
	}

	void _ctor(BootInfo& bootInfo)
	{
		// Clear uninitialized data (I'd rather it be zero than random)
		ClearBss(bootInfo);

		u64* ctorStart = &_ctorStart;
		u64* ctorEnd = &_ctorEnd;
		uint64 ctorCount = (u64(ctorEnd) - u64(ctorStart)) / sizeof(nint);

		// im doing ctors here because doing it in bootloader was a hassle
		for (nint x = 0; x < ctorCount; x++)
		{
			void(*constructor)() = (void(*)())(ctorStart[x]);
			constructor();
		}

	}

	global void KernelStart(BootInfo bootInfo)
	{
		// Run global constructors
		_ctor(bootInfo);

		// Create console
		gConsole = PrimitiveConsole(bootInfo.Framebuffer, bootInfo.font);
		gConsole.Clear();

		#ifndef NO_ANIME
		// Show loading image
		ShowLoadingImage(bootInfo);
		#endif
		
		// Initialize Hardware //
		Kernel::InitializeKernel(bootInfo);

		gConsole.WriteLine(string(OSName) + " Initialized!", Color::Green);
		gConsole.EnableCursor();

		debug("Trying to read testfile.txt");
		char* text = new char[0x100];
		nint leakage = Memory::GetRemainingHeap();

		FAT32::ReadFile(L"0:/testfile.txt", text);
		FAT32::ReadFile(L"0:/testfile.txt", text);
		
		if (FAT32::ReadFile(L"0:/testfile.txt", text) == FAT32::Status::Ok)
		{
			gConsole.Write("YAY, Contents: ");
			gConsole.WriteLine(text);
		}
		else error("FUCKING KILL ME");
		debug("Bytes leaked: %x0", leakage - Memory::GetRemainingHeap());
		delete text;


		while (true)
		{
			hlt;
		}
	}
}