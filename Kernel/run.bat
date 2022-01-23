@echo off
cd /d %~dp0
set OSNAME=WaifuOS
set BUILDDIR=bin
set OVMFDIR=../OVMFbin


qemu-system-x86_64 -device qemu-xhci -machine q35 -rtc base=localtime -drive file=%BUILDDIR%/%OSNAME%.img -m 2G -cpu qemu64,+sse2,+sse3,+sse4.1,+sse4.2 -drive if=pflash,format=raw,unit=0,file=%OVMFDIR%/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=%OVMFDIR%/OVMF_VARS-pure-efi.fd -net none -vga virtio 