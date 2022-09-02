#include <FAT32.h>

namespace FAT32
{
	bool DoesDriveContainF32(AHCI::ATAPort* drive)
	{
		// EBR is 512 bytes, 1 sector
		EBR* ebr = new EBR();
		if (!drive->Read(0, 1, ebr))
		{
			error("FAILED TO READ DRIVE!");
			delete ebr;
			return false;
		}
		
		if (ebr->BootSignature != BOOT_SIG || (ebr->Signature != 0x28 && ebr->Signature != 0x29) || ebr->SectorsPerFAT32 == 0)
		{
			delete ebr;
			return false;
		}

		// FSInfo is also 512 bytes, 1 sector
		FSInfo* fsinfo = new FSInfo();
		if (!drive->Read(ebr->FSInfoSector, 1, fsinfo))
		{
			error("FAILED TO READ DRIVE!");
			delete ebr, fsinfo;
			return false;
		}

		if (fsinfo->LeadSignature != FSInfo::LEAD_SIG || fsinfo->MidSignature != FSInfo::MID_SIG || fsinfo->TrailSignature != FSInfo::TRAIL_SIG)
		{
			delete ebr, fsinfo;
			return false;
		}

		delete ebr, fsinfo;
		return true;
	}

	FSDriver::FSDriver(AHCI::ATAPort* drive)
	{
		debug("Initializing FAT32 driver for drive #%0", drive->PortNumber);

		mBootRecord = new EBR();
		mFSInfo = new FSInfo();

		// Read boot record
		if (!drive->Read(0, 1, mBootRecord))
		{
			error("FSDriver FAILED TO READ DRIVE (Boot Record)!");
			this->~FSDriver();
			return;
		}

		// Verify boot record
		if (mBootRecord->BootSignature != BOOT_SIG || (mBootRecord->Signature != 0x28 && mBootRecord->Signature != 0x29) || mBootRecord->SectorsPerFAT32 == 0)
		{
			error("DRIVE IS NOT A VALID FAT32 VOLUME!");
			this->~FSDriver();
			return;
		}

		// Read FSInfo
		if (!drive->Read(mBootRecord->FSInfoSector, 1, mFSInfo))
		{
			error("FSDriver FAILED TO READ DRIVE! (FSInfo)");
			this->~FSDriver();
			return;
		}

		// Verify FSInfo
		if (mFSInfo->LeadSignature != FSInfo::LEAD_SIG || mFSInfo->MidSignature != FSInfo::MID_SIG || mFSInfo->TrailSignature != FSInfo::TRAIL_SIG)
		{
			error("DRIVE IS NOT A VALID FAT32 VOLUME! (FSInfo Invalid)");
			this->~FSDriver();
			return;
		}

		// Wow calculate shit
		mTotalSectors = (mBootRecord->SectorCount == 0) ? mBootRecord->LargeSectorCount : mBootRecord->SectorCount;
		mFatSizeSectors = mBootRecord->SectorsPerFAT32;
		mFirstDataSector = mBootRecord->NumReservedSectors + (mBootRecord->NumFAT * mFatSizeSectors);
		mFirstFatSector = mBootRecord->NumReservedSectors;
		mTotalDataSectors = mTotalSectors - (mBootRecord->NumReservedSectors + (mBootRecord->NumFAT * mFatSizeSectors));
		mTotalClusters = mTotalDataSectors / mBootRecord->SectorsPerCluster;
		mFirstRootDirSector = GetFirstSectorOfCluster(mBootRecord->RootClusterStart);

		if (mBootRecord->SectorSizeBytes == 0)
		{
			error("DRIVE IS NOT A VALID FAT32 VOLUME! (it's exFAT)");
			this->~FSDriver();
			return;
		}
		if (mTotalClusters < 4085)
		{
			error("DRIVE IS NOT A VALID FAT32 VOLUME! (it's FAT12)");
			this->~FSDriver();
			return;
		}
		if (mTotalClusters < 65525)
		{
			error("DRIVE IS NOT A VALID FAT32 VOLUME! (it's FAT16)");
			this->~FSDriver();
			return;
		}

		byte* clusterBuffer = calloc(mBootRecord->SectorsPerCluster * SectorSize);
		uint32 rootSectorOffset = mFirstRootDirSector;

		LFNEntry* nameEntryBuffer = new LFNEntry[256]; // kill me
		byte nameEntries = 0;

		wchar* nameBuffer = new wchar[256];

		bool keepGoing = true;
		while (keepGoing)
		{
			if (!drive->Read(rootSectorOffset, mBootRecord->SectorsPerCluster, clusterBuffer))
			{
				break;
			}
			
			DirectoryEntry* entry = (DirectoryEntry*)clusterBuffer;
			while (nint(entry) < nint(entry) + mBootRecord->SectorsPerCluster * SectorSize)
			{
				byte firstByte = entry->ShortName[0];
				if (!firstByte)
				{
					keepGoing = false;
					break;
				}
				if (firstByte == 0xE5)
				{
					entry += 1;
					continue;
				}
				if (entry->EntryAttributes == EntryAttribute::LFN)
				{
					LFNEntry* lfnEntry = (LFNEntry*)entry;
					byte idex = (lfnEntry->EntryOrder & ~(byte)(0x40)) - 1; // what the fuck?
					nameEntryBuffer[idex] = *lfnEntry;
					nameEntries++;
					entry += 1;
					continue;
				}

				if (nameEntries > 0)
				{
					uint16 nbOffset = 0;
					for (nint x = 0; x < nameEntries; x++)
					{
						LFNEntry& entry = nameEntryBuffer[x];
						cstr::wstrncpy(nameBuffer + nbOffset, entry.Name0, 5);
						nbOffset += 5;
						cstr::wstrncpy(nameBuffer + nbOffset, entry.Name1, 6);
						nbOffset += 6;
						cstr::wstrncpy(nameBuffer + nbOffset, entry.Name2, 2);
						nbOffset += 2;
					}

					if (entry->EntryAttributes & EntryAttribute::Directory)
						gConsole.Write("Folder Name (LFN): ");
					else
						gConsole.Write("File Name (LFN): ");

					char* entryName = cstr::wstrTocstr(nameBuffer, nbOffset);
					gConsole.WriteLine(entryName);

					nameEntries = 0;
					memset64(nameBuffer, 0x00, 256 << 1);
				}
				else
				{
					if (entry->EntryAttributes & EntryAttribute::Directory)
						gConsole.Write("Folder Name: ");
					else
						gConsole.Write("File Name: ");
					gConsole.WriteLine(string(entry->ShortName, 11));
				}

				entry += 1;
			}
			rootSectorOffset += mBootRecord->SectorsPerCluster * 512;
		}



	}

	FSDriver::~FSDriver()
	{
		delete mBootRecord;
		delete mFSInfo;

		mBootRecord = nullptr;
		mFSInfo = nullptr;
	}
}

