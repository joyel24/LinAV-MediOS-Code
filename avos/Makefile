# Makefile for libavos

# Copyright 2004, William Bland
# This Makefile is free software; I give unlimited permission to copy
# and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


libavos:
	arm-elf-gcc -c -mthumb -mcallee-super-interworking -fno-zero-initialized-in-bss -nostdlib -nodefaultlibs src/fonts/*.c
	arm-elf-as -o lib/libavos.o src/hardware/*.s
	mv *.o lib

.PHONY: clean
clean:
	rm -f *~ *.o *.elf *.bin

examples: src/examples/*.c
	@for i in src/examples/*.c;do\
	arm-elf-gcc -c -mthumb -mcallee-super-interworking -fno-zero-initialized-in-bss -nostdlib -nodefaultlibs -Iinclude/ $$i;\
	arm-elf-ld -Ttext 0x03000000 -e main -o `basename $$i .c`.elf `basename $$i .c`.o lib/*.o;\
	arm-elf-objcopy -O binary `basename $$i .c`.elf `basename $$i .c`.bin;\
	rm -f `basename $$i .c`.elf `basename $$i .c`.o;\
	done
