TOOL=arm-elf

GCC=$(TOOL)-gcc
AR=$(TOOL)-ar
RANLIB=$(TOOL)-ranlib

GCCFLAGS=-I../include
LDFLAGS=-elf2flt

LIB_CONTENT= \
	graphics.o \
	osd.o \
	events.o

TARGET = libGraphics.a
	
all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : fontDir $(LIB_CONTENT)
	$(AR) -rc $(TARGET) $(LIB_CONTENT) fonts/*.o
	$(RANLIB) $(TARGET)

fontDir:
	@$(MAKE) -C fonts/ all
	
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~ $(TARGET)
	@$(MAKE) -C fonts/ clean

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<