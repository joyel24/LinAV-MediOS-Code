
######################################"
# you should change this to the dir where binutil, gcc are installed
BUILDDIR=/home/thomas/Av320/arm-tools/install


LIBAVOSDIR=../includes/

LD = ld
LDPATHS = -L$(BUILDDIR)/arm-elf/lib/thumb
LDFLAGS = -Ttext `cat $(LIBAVOSDIR)/hardware.S | grep ramStart | awk '{print $$3}'` -e main -marmelf


CFLAGS = -Wall
INCLUDE = -I$(LIBAVOSDIR)/include

CC = gcc

ARFLAGS = ruv
AR = ar

OBJCOPY = objcopy
OBJCOPYFLAGS = -O binary

TARGET = arm-elf-

EXE = usb

EXEFLAGS = -mthumb -mcallee-super-interworking

LIBS = $(LIBAVOSDIR)/lib/libavos.a

all: $(EXE)

$(EXE): $(EXE).bin
	-rm -f $(EXE).elf $(EXE).o

%.o: %.c
	$(TARGET)$(CC) $(CFLAGS) $(EXEFLAGS) -c -I./ $(INCLUDE) $<

%.elf: %.o
	$(TARGET)$(LD) $(LDPATHS) $(LDFLAGS) -o $@ $< $(LIBS) -lc -lgcc

%.bin: %.elf
	$(TARGET)$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

clean:
	-rm -f $(EXE).bin

