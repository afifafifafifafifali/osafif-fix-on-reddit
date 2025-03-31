kernel_source_files := $(shell find src/impl/kernel -name *.c)
kernel_object_files := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

x86_64_c_source_files := $(shell find src/impl/x86_64 -name *.c)
x86_64_c_object_files := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.o, $(x86_64_c_source_files))

x86_64_asm_source_files := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_source_files))

x86_64_object_files := $(x86_64_c_object_files) $(x86_64_asm_object_files)

all: clean build-x86_64 

$(kernel_object_files): build/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
	gcc  -nostdlib -m64 -c -I src/intf -ffreestanding $(patsubst build/kernel/%.o, src/impl/kernel/%.c, $@) -o $@

$(x86_64_c_object_files): build/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
	gcc -nostdlib -m64 -c -I src/intf -ffreestanding $(patsubst build/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64: $(kernel_object_files) $(x86_64_object_files)
	mkdir -p dist/x86_64 && \
	ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(x86_64_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

run:
	qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
clean:
	rm -rf build dist	

disk: all
	# Step 1: Create a blank disk image
	dd if=/dev/zero of=disk.img bs=512 count=131072

	# Step 2: Create a partition table (MSDOS)
	parted disk.img --script mklabel msdos
	parted disk.img --script mkpart primary fat32 2048s 100%

	# Step 3: Create a FAT32 filesystem
	mkfs.vfat -F32 -n "AFIFOS" disk.img

	# Step 4: Copy boot files using Mtools (without mounting or losetup)
	mcopy -i disk.img targets/x86_64/iso/boot/* ::/

	# Step 5: Manually create a GRUB core image
	mkdir -p images
	grub-mkimage -O i386-pc -o images/boot.img --prefix="(hd0,msdos1)/boot/grub" normal part_msdos fat multiboot

	# Step 6: Embed the GRUB boot sector into the disk image
	dd if=images/boot.img of=disk.img bs=440 count=1 conv=notrunc

	# Step 7: Manually copy GRUB files into disk image
	mkdir -p /mnt/disk/boot/grub
	cp /usr/lib/grub/i386-pc/* /mnt/disk/boot/grub/

	# Done! Now you can test the image with QEMU:
	# qemu-system-x86_64 -drive file=disk.img,format=raw
