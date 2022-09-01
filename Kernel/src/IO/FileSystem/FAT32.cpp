#include <FAT32.h>

namespace FAT32
{
	// TODO: this
	bool DoesDriveContainF32(AHCI::AHCIDriver* drive)
	{
		byte* buffer = new byte[0x200];
		if (!drive->Read(0, 0, 1, buffer))
		{
			error("FAILED TO READ DRIVE!");
			delete buffer;
			return false;
		}

		EBR* ebr = (EBR*)buffer;
		
		debug("ebr->Signature: %x0", ebr->Signature);

		FSInfo* fsinfo = new FSInfo();

		if (!drive->Read(0, ebr->FSInfoSector, 1, fsinfo))
		{
			error("FAILED TO READ DRIVE!");
			delete buffer, fsinfo;
			return false;
		}



		delete buffer, fsinfo;
		return true;
	}

	// TODO: also this
	FSDriver::FSDriver(AHCI::AHCIDriver* drive)
	{
	}
}

