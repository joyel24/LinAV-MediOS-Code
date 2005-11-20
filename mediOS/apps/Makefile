include ../Makefile.conf


ASFLAGS  += -I../include
#-mfpu=softfpa
GCCFLAGS += -I../include

#GCCFLAGS +=  -mno-long-calls

TARGET = othello

OBJS = othello.o
#OBJS = othello_sdram.o othello_iram.o

LDFLAGS = -Tapps.lds -L../ -lLmedios  --emit-relocs -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ -lgcc
#-Wl,-Map test.map -Wl,--embedded-relocs -Wl,-s --emit-relocs
# -Wl, -elf2flt
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

all: $(OBJS)
#	$(GCC) -o $(TARGET).grv $(TARGET).c $(GCCFLAGS) $(LDFLAGS)
	$(LD)  $(OBJS) -o $(TARGET).grv $(LDFLAGS)
	@rm -f .elf *.gdb *.elf2flt

stdalone: LDFLAGS = -T../$(TARGET_DEVICE)_mediOS.lds -L../  -L/usr/local/toolchain_arm_nofpu/arm-linux-uclibc/lib/ 
stdalone: $(OBJS)
	$(LD) $(LDFLAGS) -lKboot $(OBJS) -o $(TARGET).elf -lKmedios -lgcc
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
        
clean:  
	@rm -f *.o *.bak *~ *.elf *.elf2flt $(TARGET).grv *.gdb

include ../default.rule