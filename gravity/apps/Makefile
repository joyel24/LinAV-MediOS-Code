include ../Makefile.conf

ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

LDFLAGS = -L../ -lgravity -Wl, -elf2flt
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

all: 
	$(GCC) -o test.grv test.c $(GCCFLAGS) $(LDFLAGS)
	@rm -f .elf *.gdb *.elf2flt
clean:  
	@rm -f *.o *.bak *~ *.elf *.elf2flt test.grv *.gdb

