
GCC=gcc
AR=ar
RANLIB=ranlib


GCCFLAGS=-I../include 


DEST=../lib

LIB_CONTENT= \
	graphics.o \
	events.o\
	graphics_8.o

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
