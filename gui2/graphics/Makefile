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
LIB_CONTENT += osd.o events.o misc.o debug.o sound.o 
else
LIB_CONTENT += events_X11.o misc_X11.o debug.o sound_X11.o 
endif

TARGET = libGraphics.a

SUBDIRS = fonts
	
all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : subdirs $(LIB_CONTENT)
	@$(AR) -rc $(TARGET) $(LIB_CONTENT) fonts/*.o
	@$(RANLIB) $(TARGET)
	@mv $(TARGET) $(DEST)

subdirs:        
	@@for p in $(SUBDIRS); do \
		echo '-----   building ' $$p; \
		make -C $$p --no-print-directory -s all; \
	done

clean-subdirs:
	@@for p in $(SUBDIRS); do \
		echo '-----   cleaning ' $$p; \
		make -C $$p --no-print-directory clean; \
	done
	
clean: clean-subdirs
	@echo "----- graphics dir cleaned"
	@rm -f *.o *.bak *~
	@rm -f $(DEST)/$(TARGET)
	@rm -f ../include/*~

.c.o:
	@$(GCC) $(GCCFLAGS) -c -o $@ $<
