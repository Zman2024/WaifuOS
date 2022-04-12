OSNAME = WaifuOS

GNUEFI = ../Bootloader
OVMFDIR = ../OVMFbin
LDS = kernel.ld
GC = gcc
LD = ld
ASMC = nasm

SRCDIR := src
OBJDIR := obj
BUILDDIR = bin

# Ok GCC 4.8+ if you wont let me turn off -fpermissive warnings then I'll
# disable ALL warnings. and that definetly won't cause more issues than just
# allowing me to do what i want... riiiight?
CXXFLAGS = -g -ffreestanding -fshort-wchar -pipe -mno-red-zone -O0 -fno-exceptions -fno-rtti -masm=intel -fpermissive -w -I$(SRCDIR)/Headers -c 
CFLAGS = -g -ffreestanding -fno-exceptions -fshort-wchar -O0 -fno-rtti -I$(SRCDIR)/Headers -c 
INTFLAGS = $(CXXFLAGS) # -g -fpermissive -w -mno-red-zone -mgeneral-regs-only -ffreestanding -masm=intel -I$(SRCDIR)/Headers -c 
ASMFLAGS = -f elf64 -g 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
HPPSRC = $(call rwildcard,$(SRCDIR),*.hpp)
CSRC = $(call rwildcard,$(SRCDIR),*.c)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.cppo, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.hpp, $(OBJDIR)/%.hppo, $(HPPSRC))
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.co, $(CSRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.asmo, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link
$(OBJDIR)/%.cppo: $(SRCDIR)/%.cpp
	@ echo ===== Compiling $^ =====
	@ mkdir -p $(@D)
	$(GC) $(CXXFLAGS) $^ -o $@

# this is the only why i can get it to compile hpp as cpp files
# just... dont ask ok, im very schitzo
$(OBJDIR)/%.hppo: $(SRCDIR)/%.hpp
	@ echo ===== Compiling $^ =====
	@ mkdir -p $(@D)
	cp $^ $@.cpp
	$(GC) $(CXXFLAGS) $@.cpp -o $@
	rm $@.cpp

$(OBJDIR)/%.co: $(SRCDIR)/%.c
	@ echo ===== Compiling $^ =====
	@ mkdir -p $(@D)
	$(GC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.asmo: $(SRCDIR)/%.asm
	@ echo ===== Compiling $^ =====
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -o $@

link:
	@ echo ===== Linking =====
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img -f 1440 ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img zap-light16.psf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img loadingicon.raw ::

	cp $(BOOTEFI) $(BUILDDIR)/dsk/efi/boot
	cp $(BOOTEFI) $(BUILDDIR)/dsk/efi/boot/bootx64.efi
	cp $(BUILDDIR)/kernel.elf $(BUILDDIR)/dsk
	cp zap-light16.psf $(BUILDDIR)/dsk
	cp loadingicon.raw $(BUILDDIR)/dsk

run:
	qemu-system-x86_64 -device qemu-xhci -machine q35 -rtc base=localtime -drive file=$(BUILDDIR)/$(OSNAME).img -m 2G -cpu qemu64,+sse2,+sse3,+sse4.1,+sse4.2 -drive if=pflash,format=raw,unit=0,file=$(OVMFDIR)/OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=$(OVMFDIR)/OVMF_VARS-pure-efi.fd -net none -vga virtio
