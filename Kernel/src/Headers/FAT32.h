#pragma once
#ifndef H_FAT32
#define H_FAT32
#include <Types.h>
#include <AHCI.hpp>
#include <Memory.h>
#include <List.h>

namespace FAT32
{
	constexpr uint16 BOOT_SIG = 0xAA55;
	constexpr uint32 SectorSize = 0x200;

	namespace EntryAttribute
	{
		enum : byte
		{
			ReadOnly = 0x01,
			Hidden = 0x02,
			System = 0x04,
			VolumeID = 0x08,
			Directory = 0x10,
			Archive = 0x20,
			LFN = (ReadOnly | Hidden | System | VolumeID),
		};
	}

	// BIOS Parameter Block
	struct BPB
	{
		byte CodeJMP[3]; // this should be "jmp short (somewhere); nop;"
		char OEM[8]; // OEM Identifier

		uint16 SectorSizeBytes;
		byte SectorsPerCluster;
		uint16 NumReservedSectors;
		byte NumFAT; // Number of File Allocation Tables (FAT's) on the storage media.

		uint16 NumRootDirs;	// Number of root directory entries
		uint16 SectorCount; // The total sectors in the logical volume. If this value is 0, it means there are more than 65535 sectors in the volume
		byte MediaDescriptorType;
		uint16 SectorsPerFAT16; // unused in fat32

		uint16 SectorsPerTrack;
		uint16 NumHeads; // Number of heads or sides on the storage media.
		uint32 NumHiddenSectors; // Number of hidden sectors. (i.e. the LBA of the beginning of the partition.)
		uint32 LargeSectorCount; // Large sector count. This field is set if there are more than 65535 sectors in the volume

	} packed;

	// Extended Boot Record
	struct EBR : public BPB
	{
		uint32 SectorsPerFAT32;
		uint16 Flags;
		uint16 Version; // FAT version number. The high byte is the major version and the low byte is the minor version

		uint32 RootClusterStart; // the cluster the root directory starts at
		uint16 FSInfoSector; // the sector that FSInfo is located
		uint16 BootBackupSector; // the sector that the boot sector backup is located
		byte rsv0[12];

		byte DriveType; // 0x00 for a floppy disk and 0x80 for hard disks
		byte WinNTFlags;
		byte Signature; // must be 0x28 or 0x29
		uint32 VolumeID;

		char VolumeLabel[11]; // Volume label string
		char SystemIdentifier[8]; // Should be "FAT32   ". The spec says never to trust the contents of this string for any use

		byte BootCode[420];
		uint16 BootSignature; // 0xAA55

	} packed;

	struct DirectoryEntry
	{
		char ShortName[11]; // first 8 chars = name
		
		byte EntryAttributes; // See FAT32::EntryAttribute
		byte rsvNT; // Ok mr windows nt reserving random bytes

		byte CreationTimestampTS; // Creation time in tenths of a second. Range 0-199 inclusive
		uint16 CreationTimestampHMS; // The time that the file was created. Multiply Seconds by 2.
		uint16 CreationTimestampDate; // The date on which the file was created.

		uint16 LastAccessDate; // Last accessed date. Same format as the creation date.
		uint16 EntryFirstClusterHigh; // The high 16 bits of this entry's first cluster number.

		uint16 ModificationTimestampHMS; // The time that the file was created. Multiply Seconds by 2.
		uint16 ModificationTimestampDate; // The date on which the file was created.

		uint16 EntryFirstClusterLow; // The low 16 bits of this entry's first cluster number.
		uint32 FileSize; // The size of the file in bytes.

	} packed;

	// The long file name entries are always placed immediately before their 8.3 entry
	struct LFNEntry
	{
		byte EntryOrder; // The order of this entry in the sequence of long file name entries
		wchar Name0[5]; // The first 5, 2-byte characters of this entry.
		byte LFNAttribute; // Attribute. Always equals 0x0F. (the long file name attribute)
		byte EntryType; // 	Long entry type. Zero for name entries.

		byte ShortChecksum; // Checksum generated of the short file name when the file was created
		wchar Name1[6]; // The next 6, 2-byte characters of this entry.
		uint16 Zero; // Always zero.
		wchar Name2[2]; // The final 2, 2-byte characters of this entry.

	} packed;

	// File System Info struct
	struct FSInfo
	{
		static constexpr uint32 LEAD_SIG = 0x41615252;
		static constexpr uint32 MID_SIG = 0x61417272;
		static constexpr uint32 TRAIL_SIG = 0xAA550000;

		uint32 LeadSignature; // must be 0x41615252 to indicate a valid FSInfo structure
		byte rsv0[480];

		uint32 MidSignature; // Another signature (must be 0x61417272) <--- WYSI!?!?!?
		uint32 LastFreeCluster; // Contains the last known free cluster count on the volume. If the value is 0xFFFFFFFF, then the free count is unknown
		uint32 SearchFreeCluster; // Indicates the cluster number at which the filesystem driver should start looking for available clusters

		byte rsv1[12];
		uint32 TrailSignature; // 0xAA550000

	} packed;

	extern bool DoesDriveContainF32(AHCI::ATAPort* drive);

	class FSDriver
	{
	public:
		FSDriver(AHCI::ATAPort* drive);

		~FSDriver();

	private:
		uint32 mTotalSectors;
		uint32 mFatSizeSectors;
		uint32 mFirstDataSector;
		uint32 mFirstFatSector;
		uint32 mFirstRootDirSector;
		uint32 mTotalDataSectors;
		uint32 mTotalClusters;

		EBR* mBootRecord;
		FSInfo* mFSInfo;



		forceinline uint32 GetFirstSectorOfCluster(uint32 cluster)
		{
			return ((cluster - 2) * mBootRecord->SectorsPerCluster) + mFirstDataSector;
		}

	};

}


#endif