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
	arm-linux-gcc -Wa,-R -c -mthumb-interwork -fno-zero-initialized-in-bss -Iinclude/ src/fonts/*.c
	arm-linux-gcc -Wa,-R -c -mthumb-interwork -fno-zero-initialized-in-bss -Iinclude/ src/drivers/*.c
	arm-linux-as -mfpu=softfpa -mthumb-interwork -o lib/libavos.o src/hardware/*.s
	mv *.o lib

.PHONY: clean
clean:
	rm -f *~ *.o *.elf *.bin

examples: src/examples/*.c
	@for i in src/examples/*.c;do\
	arm-linux-gcc -mthumb-interwork -static-libgcc -c -Wa,-R\
    -fno-zero-initialized-in-bss -Iinclude/ $$i;\
	arm-linux-ld -N -Ttext 0x03000000 -e main -o `basename $$i .c`.elf `basename $$i .c`.o\
    lib/*.o -lfloat -lgcc -L/home/jimmy/archos/uclib/toolchain/gcc-3.3.x/toolchain_arm_nofpu/lib/gcc-lib/arm-linux-uclibc/3.3.2/;\
	arm-linux-objcopy -O binary `basename $$i .c`.elf `basename $$i .c`.bin;\
    rm -f `basename $$i .c`.elf `basename $$i .c`.o;\
    done
