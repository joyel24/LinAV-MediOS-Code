# Makefile for libavos

# Copyright 2004, William Bland
# This Makefile is free software; I give unlimited permission to copy
# and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


test: test.c
	arm-elf-gcc -c -mthumb -mcallee-super-interworking -nostdlib -nodefaultlibs -Iinclude test.c
	arm-elf-ld -Ttext 0x03000000 -e main -o test.elf test.o lib/libavos.o
	arm-elf-objcopy -O binary test.elf test.bin
	rm -f test.elf test.o
