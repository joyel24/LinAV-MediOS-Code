include ../../Makefile.conf

#GCCFLAGS = -Wall -mlong-calls  -nostdlib -fno-builtin  -mcpu=arm7tdmi -Os

ASFLAGS  += -I./include -I../../include -I. 
#-mfpu=softfpa
GCCFLAGS += -I./include -I../../include -I. 

#GCCFLAGS +=  -mno-long-calls

#TOOL=arm-linux
#GCC=$(TOOL)-gcc
#AS=$(TOOL)-as
#LD=$(TOOL)-ld
#OBJCOPY= $(TOOL)-objcopy
#RM=rm
#AR=$(TOOL)-ar
#RANLIB=$(TOOL)-ranlib

TARGET = avboy

SRC = $(wildcard *.c)
SRC2 = $(wildcard *.S)
OBJS = $(SRC:%.c=%.o) $(SRC2:%.S=%.o)

SUBDIRS =



LDFLAGS =  -nostdlib --emit-relocs -L../../ -lLmedios -TAV3XX_aoboy.lds
#-Wl,-Map test.map -Wl,--embedded-relocs -Wl,-s --emit-relocs
# -Wl, -elf2flt -Tapps.lds
#LDFLAGS = -L/home/christophe/gcc-3.3.x/toolchain_arm_nofpu/arm-linux/lib/ -lgcc

all: subdirs $(OBJS)
	@cp -f $(OBJS) build
#	$(GCC) build/*.o -o $(TARGET).grv  $(LDFLAGS) 
	$(LD)  build/*.o -o $(TARGET).med $(LDFLAGS) -Map avboy.map
	$(LD)  build/*.o -o $(TARGET).elf -nostdlib -L../../ -lLmedios -TAV3XX_aoboy.lds
	@rm -f .elf *.gdb *.elf2flt
        
stdalone: subdirs $(OBJS)
	@cp -f $(OBJS) build
	@$(AR) -rc libavboy.a build/*.o
	@$(RANLIB) libavboy.a

clean:   clean-subdirs
	@rm -f *.o *.bak *~ *.elf *.elf2flt $(TARGET).grv *.gdb build/*

include ../../default.rule