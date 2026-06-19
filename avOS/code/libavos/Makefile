# Makefile for libavos

# Copyright 2004, William Bland
# This Makefile is free software; I give unlimited permission to copy
# and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


all:
	@if test -a hardware.S; then \
		arm-elf-as -o libavos.o libavos.S; \
	else \
		echo "You must link hardware.S to jbmm.S or av300.S first"; \
	fi

install: libavos.o
	cp libavos.o /usr/local/lib/
	cp libavos.h /usr/local/include/

.PHONY: clean
clean:
	rm -f *~ *.o *.elf *.bin

hello: hello.c
	arm-elf-gcc -c -mthumb -mcallee-super-interworking -nostdlib -nodefaultlibs -I/usr/local/include/ hello.c
	arm-elf-ld -Ttext `cat hardware.S | grep ramStart | awk '{print $$3}'` -e main -o hello.elf hello.o /usr/local/lib/libavos.o
	arm-elf-objcopy -O binary hello.elf hello.bin
	rm -f hello.elf hello.o
