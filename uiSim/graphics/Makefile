
GCC=gcc
AR=ar
RANLIB=ranlib

GCCFLAGS=-I../include 


DEST=../lib

LIB_CONTENT= \
	graphics.o \

TARGET = libGraphics.a
	
all: $(TARGET)
force: clean $(TARGET)

$(TARGET) : $(LIB_CONTENT)
	$(AR) -rc $(TARGET) $(LIB_CONTENT)
	$(RANLIB) $(TARGET)
	@mv $(TARGET) $(DEST)

clean:
	@echo "**** Cleaning..."
	@rm -f *.o *.bak *~
	@rm -f $(DEST)/$(TARGET)
	@rm -f ../include/*~

.c.o:
	$(GCC) $(GCCFLAGS) -c -o $@ $<
