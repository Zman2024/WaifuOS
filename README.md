# WaifuOS
Operating system for my waifu ♥

## Introduction
This operating system is a bit of a joke, as well as an opportunity for me to learn about OS development.

WaifuOS is a x86-64 operating system so make sure you have correct CPU dummy >~<

## Compiling from source
### Linux Users

Do the git clone thingy in console haha ok good.

Compilation (Bootloader): 

  1. cd to Bootloader
  2. do make all
  3. wow cool

Compilation (Kernel): 
  
  1. cd to Kernel
  2. Do ./setup
  3. Do ./build
  4. Image can be found in Kernel/bin/WaifuOS.img it can be flashed to a USB and booted as an EFI application\*

### Windows Users
#### Note: You must have WSL installed

Clone the repo (Code -> Download ZIP) and extract it somewhere

Compilation (Bootloader): 

  1. cd to Bootloader
  2. Open cmd
  3. Run wsl make all
  4. Wow cool

Compilation (Kernel): 
  
  1. Navigate to Kernel
  2. Run setup.bat
  3. Run build.bat
  4. Image can be found in Kernel/bin/WaifuOS.img it can be flashed to a USB and booted as an EFI application\*

## Running in QEMU
Windows users can (assuming you have QEMU installed correctly) simply double click run.bat in the Kernel folder

Linux users:

  1. cd into Kernel
  2. Do make run
  3. waifus (maybe)

# I'm getting tired of writing a readme so im done
\*The image is in FAT format and might not work correctly, you should just copy all from bin/dsk to the root of a FAT32 formatted USB, that's what works for me. Don't use NTFS most BIOSes will shit themselves
