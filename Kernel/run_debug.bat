@echo off
cd /d %~dp0
set OSNAME=WaifuOS
set BUILDDIR=bin
set OVMFDIR=../OVMFbin

start qemu-system-x86_64 ^
 --bios "%OVMFDIR%/OVMF_CODE-pure-efi.fd" ^
 -device qemu-xhci ^
 -machine q35 ^
 -rtc base=localtime ^
 -m 16G ^
 -cpu qemu64,+mmx,+sse,+sse2,+sse3,+sse4.1,+sse4.2,+rdrand,+xsave,+aes,+avx,+avx2,enforce ^
 -smp cores=6 ^
 -net none ^
 -vga virtio ^
 -smp 6 ^
 -s ^
 -S ^
 %BUILDDIR%/%OSNAME%.img

C:/Windows/SysNative/wsl.exe gdb bin/kernel.elf