include ../Makefile.conf


ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

#GCCFLAGS +=  -mno-long-calls

TARGET = othello
OBJS = othello.o

LDFLAGS = -L../ -lLmedios -Wl, -elf2flt  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc

all: $(OBJS)
#	$(GCC) -o $(TARGET).grv $(TARGET).c $(GCCFLAGS) $(LDFLAGS)
	@rm -f .elf *.gdb *.elf2flt
        
clean:  
	@rm -f *.o *.bak *~ *.elf *.elf2flt $(TARGET).grv *.gdb

include ../default.rule