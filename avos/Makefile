# Makefile for libavos

# Copyright 2004, William Bland
# This Makefile is free software; I give unlimited permission to copy
# and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

BUILDDIR=/home/jimmy/archos/uclib/toolchain/gcc-3.3.x/toolchain_arm_nofpu
LDFLAG=-L$(BUILDDIR)/arm-linux/lib

##########################
## you need to make some special link:
## in LDFLAG dir do ln -s ../../lib/gcc-lib/arm-linux-uclibc/3.3.2/libgcc.a
## and ln -s ../../lib/libfloat.a
## or find a better way to do it ;)


libavos:
	arm-linux-gcc -Wa,-R -c -mthumb-interwork -fno-zero-initialized-in-bss -Iinclude/ src/fonts/*.c
	arm-linux-gcc -Wa,-R -c -mthumb-interwork -fno-zero-initialized-in-bss -Iinclude/ src/drivers/*.c
	arm-linux-as -mfpu=softfpa -mthumb-interwork -o lib/libavos.o src/hardware/*.s
	mv *.o lib

.PHONY: clean
clean:
	rm -f *~ *.o *.elf *.bin
	rm -f lib/*.o
	rm -f dist/*.bin

examples: src/examples/*.c
	@for i in src/examples/*.c;do\
	arm-linux-gcc -mthumb-interwork -static-libgcc -c -Wa,-R\
    -fno-zero-initialized-in-bss -Iinclude/ $$i;\
	arm-linux-ld -N -Ttext 0x03000000 -e main -o `basename $$i .c`.elf `basename $$i .c`.o\
    lib/*.o $(LDFLAG) -lc -lfloat -lgcc;\
	arm-linux-objcopy -O binary `basename $$i .c`.elf `basename $$i .c`.bin;\
    rm -f `basename $$i .c`.elf `basename $$i .c`.o;\
    mv *.bin dist/;\
    done
    
debug: src/examples/*.c
	@for i in src/examples/*.c;do\
	arm-linux-gcc -mthumb-interwork -static-libgcc -c -Wa,-R\
    -fno-zero-initialized-in-bss -Iinclude/ $$i;\
	arm-linux-ld -N -Ttext 0x03000000 -e main -o `basename $$i .c`.elf `basename $$i .c`.o\
    lib/*.o $(LDFLAG) -lc -lfloat -lgcc;\
	arm-linux-objcopy -O binary `basename $$i .c`.elf `basename $$i .c`.bin;\
    done

boot: src/boot/boot.c
	arm-linux-as -mfpu=softfpa -mthumb-interwork -o bootstart.o src/boot/bootstart.s
	arm-linux-gcc -mthumb-interwork -static-libgcc -c -Wa,-R\
    -fno-zero-initialized-in-bss -Iinclude/ src/boot/boot.c
	arm-linux-ld -N -Ttext 0x03400000 -e main2 -o boot.elf bootstart.o boot.o\
    lib/*.o $(LDFLAG) -lc -lfloat -lgcc
	arm-linux-objcopy -O binary boot.elf boot.bin
	rm -f boot.elf boot.o
	mv boot.bin dist/
    
