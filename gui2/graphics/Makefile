TOOL=arm-elf

GCC=$(TOOL)-gcc
AR=$(TOOL)-ar
RANLIB=$(TOOL)-ranlib

GCCFLAGS=-I../include -I../jpeg-6b
LDFLAGS=-elf2flt

DEST=../lib

LIB_CONTENT= \
	graphics.o \
        graphics_8.o \
	osd.o \
	events.o \
        misc.o

TARGET = libGraphics.a
	
all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : fontDir $(LIB_CONTENT)
	$(AR) -rc $(TARGET) $(LIB_CONTENT) fonts/*.o
	$(RANLIB) $(TARGET)
	@mv $(TARGET) $(DEST)

fontDir:
	@$(MAKE) -C fonts/ all
	
clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~
	@rm -f $(DEST)/$(TARGET)
	@rm -f ../include/*~
	@$(MAKE) -C fonts/ clean

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
