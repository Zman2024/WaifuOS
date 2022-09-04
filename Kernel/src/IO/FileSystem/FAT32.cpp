#include <FAT32.h>

namespace FAT32
{
	List<FSDriver*> Drives = List<FSDriver*>();

	bool GetEntryInfo(const wchar* path, EntryInfo& info)
	{
		nint pathLength = cstr::wstrlen(path);
		if (pathLength < 2)
		{
			return false;
		}
		byte driveNum = (byte)(path[0]);
		{
			char dnums[2];
			dnums[0] = driveNum;
			dnums[1] = 0x0;
			driveNum = cstr::ToInt(dnums, true);
		}

		FSDriver* drive = nullptr;
		for (nint x = 0; x < Drives.GetCount(); x++)
		{
			drive = Drives[x];
			if (drive->GetID() == driveNum)
				break;
			drive = nullptr;
		}

		if (drive == nullptr)
		{
			return false;
		}

		wchar* pathOffset = path + 2; // first 2 chars are "{driveNum}:"
		pathLength -= 2;

		// if the length is now == 0 then they want the root dir info

		return drive->GetEntryInfo(pathOffset, info);
	}

	bool ReadFile(const wchar* path, vptr out)
	{
		EntryInfo info;
		if (!GetEntryInfo(path, info))
		{
			warn("didnt find the file");
			return false;
		}

		return ReadFile(info, out);
	}

	bool ReadFile(const EntryInfo& info, vptr out)
	{
		FSDriver* drive = nullptr;
		for (nint x = 0; x < Drives.GetCount(); x++)
		{
			drive = Drives[x];
			if (drive->GetID() == info.DriveID)
				break;
			drive = nullptr;
		}

		if (drive == nullptr)
		{
			return false;
		}

		
		return drive->ReadFile(info, out);
	}

	bool DoesDriveContainF32(AHCI::ATAPort* drive)
	{
		// EBR is 512 bytes, 1 sector
		EBR* ebr = new EBR();

		// FSInfo is also 512 bytes, 1 sector
		FSInfo* fsinfo = new FSInfo();

		if (!drive->Read(0, 1, ebr))
		{
			error("FAILED TO READ DRIVE!");
			delete ebr, fsinfo;
			return false;
		}
		
		if (ebr->BootSignature != BOOT_SIG || (ebr->Signature != 0x28 && ebr->Signature != 0x29) || ebr->SectorsPerFAT32 == 0)
		{
			delete ebr, fsinfo;
			return false;
		}

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

	// Remember to free the name when you're done
	wchar* GetEntryNameLFN(List<LFNEntry>& Entries)
	{
		nint entryCount = Entries.GetCount();
		nint nameLen = (entryCount * 13) + 1;
		wchar* name = new wchar[nameLen];

		uint16 nameOffset = 0;
		for (nint x = 0; x < entryCount; x++)
		{
			LFNEntry& entry = Entries[x];
			byte order = (entry.EntryOrder & ~(byte)(0x40)) - 1; // what the fuck?
			nameOffset = (order * 13);

			cstr::wstrncpy((name + nameOffset + 0), entry.Name0, 5);
			cstr::wstrncpy((name + nameOffset + 5), entry.Name1, 6);
			cstr::wstrncpy((name + nameOffset + 11), entry.Name2, 2);
		}

		name[nameLen - 1] = 0x00;
		return name;
	}

	// Remember to free the name when you're done (returns lowercase)
	char* GetEntryName(DirectoryEntry& entry)
	{
		char* buffer = new char[13];
		buffer[12] = 0x00;
		char* temp = buffer;

		int64 index = cstr::IndexOf(entry.ShortName, ' ', 8);
		if (index < 0 || index >= 8)
			index = 8;

		memcpy(temp, entry.ShortName, index);
		temp += index;
		if (entry.ShortExt[0] != ' ')
		{
			temp[0] = '.';
			temp += 1;

			for (byte x = 0; x < sizeof(entry.ShortExt); x++)
			{
				if (entry.ShortExt[x] == ' ') break;
				temp[0] = entry.ShortExt[x];
				temp += 1;
			}
			temp[0] = 0x00;
		}
		else temp[0] = 0x00;
		cstr::ToLower(buffer);
		return buffer;
	}

	EntryInfo FromEntry(DirectoryEntry& base, byte driveID, wchar* lfn)
	{
		EntryInfo ret = EntryInfo();
		{
			ret.EntryAttributes = base.EntryAttributes;
			
			ret.CreationTimestampTS = base.CreationTimestampTS;
			ret.CreationTimestampHMS = base.CreationTimestampHMS;
			ret.CreationTimestampDate = base.CreationTimestampDate;

			ret.LastAccessDate = base.LastAccessDate;

			ret.ModificationTimestampHMS = base.ModificationTimestampHMS;
			ret.ModificationTimestampDate = base.ModificationTimestampDate;

			ret.EntryFirstClusterLow = base.EntryFirstClusterLow;
			ret.EntryFirstClusterHigh = base.EntryFirstClusterHigh;

			ret.FileSize = base.FileSize;

			ret.wEntryName = lfn;

			ret.DriveID = driveID;

			memcpy(ret.ShortName, base.ShortName, sizeof(base.ShortName));
			memcpy(ret.ShortExt, base.ShortExt, sizeof(base.ShortExt));
		}

		return ret;
	}

	uint16 FSDriver::sNextDriveID = 0;

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

		mDrive = drive;
		mDriveID = sNextDriveID++;
		Drives.Add(this);
	}

	FSDriver::~FSDriver()
	{
		delete mBootRecord;
		delete mFSInfo;

		mBootRecord = nullptr;
		mFSInfo = nullptr;
		mDrive = nullptr;

		Drives.Remove(this);
	}

	bool FSDriver::FindEntry(const wchar* name, EntryInfo& out, EntryInfo* root)
	{
		if (root && (!(root->EntryAttributes & EntryAttribute::Directory) || root->DriveID != mDriveID))
		{
			return false;
		}

		List<LFNEntry> nameEntries = List<LFNEntry>();

		uint32 clusterSize = mBootRecord->SectorsPerCluster * SectorSize;
		DirectoryEntry* clusterBuffer = (DirectoryEntry*)new byte[clusterSize];
		uint32 entriesPerCluster = clusterSize / sizeof(DirectoryEntry);
		uint32 startCluster = mBootRecord->RootClusterStart;

		if (root) startCluster = ((root->EntryFirstClusterLow) | u32(root->EntryFirstClusterHigh) << 16);

		// I rarely use do; while; but here it's fine
		do
		{
			if (!ReadCluster(startCluster, clusterBuffer))
			{
				delete clusterBuffer;
				return false;
			}

			for (nint x = 0; x < entriesPerCluster; x++)
			{
				DirectoryEntry* entry = clusterBuffer + x;
				switch (entry->ShortName[0])
				{
					case ENTRY_END:
					{
						delete clusterBuffer;
						return false;
					}

					case ENTRY_DELETED:
						continue;
				}

				if (entry->EntryAttributes == EntryAttribute::LFN)
				{
					LFNEntry* lfnEntry = (LFNEntry*)entry;
					nameEntries.Add(*lfnEntry);
					continue;
				}

				if (nameEntries.GetCount() > 0)
				{
					wchar* lfnName = GetEntryNameLFN(nameEntries);
					nint len = cstr::wstrlen(lfnName);
					wchar* lfnNameLower = new wchar[len];

					cstr::wstrncpy(lfnNameLower, lfnName, len);
					cstr::wstrToLower(lfnNameLower);

					if (cstr::wstrcmp(name, lfnNameLower)) // we found it
					{
						out = FromEntry(*entry, mDriveID, lfnName); // we dont deallocate lfnName, it belongs to caller now
						delete clusterBuffer, lfnNameLower;
						return true;
					}

					nameEntries.Clear();
					delete lfnName, lfnNameLower, clusterBuffer;
					continue;
				}

				// the entry doesn't have any LFNs
				char* entryName = GetEntryName(*entry);

				wchar* wEntryName = cstr::cstrTowstr(entryName, strlen(entryName));
				delete entryName;
				if (cstr::wstrcmp(name, wEntryName))
				{
					// we found it
					out = FromEntry(*entry, mDriveID, nullptr);
					delete clusterBuffer;
					return true;
				}

			}

		} 
		while (true);


	}

	bool FSDriver::ReadCluster(uint32 cluster, vptr buffer)
	{
		uint64 sector = GetFirstSectorOfCluster(cluster);
		if (!mDrive->Read(sector, mBootRecord->SectorsPerCluster, buffer))
		{
			return mDrive->Read(sector, mBootRecord->SectorsPerCluster, buffer);
		}
		return true;
	}

	bool FSDriver::ReadFile(const EntryInfo& info, vptr out)
	{
		uint32 sector = GetFirstSectorOfCluster(info.EntryFirstClusterLow | u32(info.EntryFirstClusterHigh) << 16);

		if (!mDrive->ReadBytes(sector, info.FileSize, out))
		{
			return mDrive->ReadBytes(sector, info.FileSize, out);
		}

		return true;
	}

	void FSDriver::PrintRootDir()
	{
		byte* clusterBuffer = new byte[mBootRecord->SectorsPerCluster * SectorSize];
		uint32 rootSectorOffset = mFirstRootDirSector;

		List<LFNEntry> nameEntries = List<LFNEntry>();

		bool keepGoing = true;
		while (keepGoing)
		{
			if (!mDrive->Read(rootSectorOffset, mBootRecord->SectorsPerCluster, clusterBuffer))
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
					nameEntries.Add(*lfnEntry);
					entry += 1;
					continue;
				}

				if (nameEntries.GetCount() > 0)
				{
					wchar* lFileName = GetEntryNameLFN(nameEntries);

					if (entry->EntryAttributes & EntryAttribute::Directory)
						gConsole.Write("Folder Name (LFN): ");
					else
						gConsole.Write("File Name (LFN): ");

					char* entryName = cstr::wstrTocstr(lFileName, nameEntries.GetCount() * 13);
					gConsole.WriteLine(entryName);

					nameEntries.Clear();

					delete lFileName;
				}
				else
				{
					if (entry->EntryAttributes & EntryAttribute::Directory)
						gConsole.Write("Folder Name: ");
					else
						gConsole.Write("File Name: ");
					char* name = GetEntryName(*entry);
					gConsole.WriteLine(name);
					delete name;
				}

				entry += 1;
			}
			rootSectorOffset += mBootRecord->SectorsPerCluster * 512;
		}

		delete clusterBuffer;
	}

	bool FSDriver::GetEntryInfo(const wchar* path, EntryInfo& info)
	{
		nint wlen = cstr::wstrlen(path);
		if (path[0] == L'/' || path[0] == L'\\')
		{
			path += 1;
			wlen -= 1;
		}

		if (wlen == 0 || wlen == -1L) return false;

		if (path[wlen - 1] == L'/' || path[wlen - 1] == L'\\')
		{
			wlen -= 1;
			if (wlen == 0) return false;
		}

		// Root dir
		EntryInfo currentEntry = EntryInfo();
		{
			currentEntry.EntryFirstClusterLow = mBootRecord->RootClusterStart;
			currentEntry.EntryFirstClusterHigh = mBootRecord->RootClusterStart >> 16;
			currentEntry.EntryAttributes = EntryAttribute::Directory;
			currentEntry.DriveID = mDriveID;
		}

		wchar* cleanedPath = cstr::wsubstring(path, 0, wlen); // make copy (free this)
		wchar* currentPath = cleanedPath; 

		cstr::wstrToLower(currentPath);

		do
		{
			int64 indexOfSlash = cstr::wstrIndexOf(currentPath, L'/');

			if (indexOfSlash < 0)
			{
				indexOfSlash = cstr::wstrIndexOf(currentPath, L'\\');
				if (indexOfSlash < 0) // they are looking for something in the current directory
				{
					bool ret = FindEntry(currentPath, currentEntry, &currentEntry);
					info = currentEntry;

					delete cleanedPath;
					return ret; // found it (or doesn't exist)
				}
			}

			wchar* ffname = cstr::wsubstring(currentPath, 0, indexOfSlash);
			if (!FindEntry(ffname, currentEntry, &currentEntry))
			{
				delete cleanedPath, ffname;
				return false;
			}

			delete ffname;

			currentPath += indexOfSlash + 1;
			wlen -= indexOfSlash + 1;
		} 
		while (true);

	}

}

