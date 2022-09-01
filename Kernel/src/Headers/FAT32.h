#pragma once
#ifndef H_FAT32
#define H_FAT32
#include <Types.h>
#include <AHCI.hpp>
#include <Memory.h>

namespace FAT32
{
	// BIOS Parameter Block
	struct BPB
	{
		byte CodeJMP[3]; // this should be "jmp short (somewhere); nop;"
		char OEM[8]; // OEM Identifier

		uint16 BytesPerSector;
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

	// File System Info struct
	struct FSInfo
	{
		uint32 LeadSignature; // must be 0x41615252 to indicate a valid FSInfo structure
		byte rsv0[480];

		uint32 MidSignature; // Another signature (must be 0x61417272) <--- WYSI!?!?!?
		uint32 LastFreeCluster; // Contains the last known free cluster count on the volume. If the value is 0xFFFFFFFF, then the free count is unknown
		uint32 SearchFreeCluster; // Indicates the cluster number at which the filesystem driver should start looking for available clusters

		byte rsv1[12];
		uint32 TrailSignature; // 0xAA550000

	} packed;

	extern bool DoesDriveContainF32(AHCI::AHCIDriver* drive);

	class FSDriver
	{
	public:
		FSDriver(AHCI::AHCIDriver* drive);

	};

}


#endif