# General config and build tools
TOOL=arm-linux

GCC=$(TOOL)-gcc
LD=$(TOOL)-ld
AR=$(TOOL)-ar
AS=$(TOOL)-as
OBJCOPY= $(TOOL)-objcopy
RANLIB=$(TOOL)-ranlib
RM=rm
CP=cp
MV=mv

# State the main target to build

TARGET = avlo
BUILD_DIR = ../build

#LOCAL FLAGS
GCCFLAGS = -Wa,-R -fno-zero-initialized-in-bss -Wall -I../include -static-libgcc
ASFLAGS  = -mfpu=softfpa
LDFLAGS  = -L../build/lib -lav -lc -lfloat -lgcc
#-lavc -lavmisc -lavhardware -lavgraphics -lavfonts  -lavata -lc -lfloat -lgcc


# State the object files that the main target is made up of
OBJS = bootstart.O avlo.o parse_cfg.o loadCJBM.o

all:$(TARGET)

# Rule to make executable
$(TARGET) : libDir $(OBJS)
	$(LD) -N -Ttext 0x03400000 -e main2 -o $(TARGET).elf $(OBJS) $(LDFLAGS)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	@$(RM) -f $(TARGET).elf
	@$(MV) -f $(TARGET).bin $(BUILD_DIR)

libDir:
	@$(MAKE) -C ../lib-src/ all
    
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.O *.bak *~ *.elf $(BUILD_DIR)/*.bin
	@$(MAKE) -C ../lib-src clean
        
.SUFFIXES: .c .S .O .o

.S.O:
	$(AS) $(ASFLAGS) -o $@ $<
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
        


# END of Makefile