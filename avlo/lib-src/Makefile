# Lib makefile 

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

TARGET=libav.a
DEST=../build/lib

SRC1 = $(wildcard *.c)
OBJS1 = $(SRC1:%.c=%.o)
SRC2 = $(wildcard *.S)
OBJS2 = $(SRC2:%.S=%.O)
OBJS=$(OBJS1) $(OBJS2)

GCCFLAGS = -Wa,-R -fno-zero-initialized-in-bss -Wall -I../include -static-libgcc
ASFLAGS  = -mfpu=softfpa

all: $(TARGET)

$(TARGET): fontDir $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS) fonts/*.o
	$(RANLIB) $(TARGET)
	@$(MV) $(TARGET) $(DEST)

fontDir:
	@$(MAKE) -C fonts/ all
        
clean:    
	@rm -f *.o *.O *.bak *~ $(BUILD_DIR)/lib/$(TARGET)
	@$(MAKE) -C fonts/ clean
        
.SUFFIXES: .c .S .O .o

.S.O:
	$(AS) $(ASFLAGS) -o $@ $<
        
.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<

# End of Makefile
