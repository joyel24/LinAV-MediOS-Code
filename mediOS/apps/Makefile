include ../Makefile.conf


ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

TARGET = test

LDFLAGS = -L../ -lmedios -Wl,-s -Wl,--emit-relocs -e _start
#-Wl,-Map test.map -Wl,--embedded-relocs -Wl,-s --emit-relocs
# -Wl, -elf2flt
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

all: 
	$(GCC) -o $(TARGET).grv $(TARGET).c $(GCCFLAGS) $(LDFLAGS)
#	$(LD) $(TARGET).o -o $(TARGET).grv $(LDFLAGS)
	@rm -f .elf *.gdb *.elf2flt
clean:  
	@rm -f *.o *.bak *~ *.elf *.elf2flt $(TARGET).grv *.gdb

