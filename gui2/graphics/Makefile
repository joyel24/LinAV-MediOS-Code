include ../Makefile.conf

GCCFLAGS += -I../include

DEST=../lib

LIB_CONTENT= \
	graphics.o \
        graphics_8.o \
        graphics_32.o

ifeq ($(HAVE_JPEG),y)
GCCFLAGS += -I../jpeg-6b
endif

ifeq ($(BUILD),AV)
LIB_CONTENT += osd.o events.o misc.o sound.o 
else
LIB_CONTENT += events_X11.o misc_X11.o sound_X11.o 
endif

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
