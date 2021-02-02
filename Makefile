CC=/home/bastian/kern/cross/bin/i686-elf-gcc

all: kernel.o boot.o

all: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot
	cp grub.cfg iso/boot/grub
	grub-mkrescue -o kernel.iso iso

kernel.bin: kernel.o boot.o
	$(CC) -T linker.ld -o kernel.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot.o: boot.s
	nasm -felf32 boot.s -o boot.o

clean:
	rm -rf *.o *.bin iso/* *.iso
