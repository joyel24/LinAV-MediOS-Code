# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavfonts.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include

# State the object files that the main target is made up of
OBJS = 4x6.o 4x8.o 5x7.o 5x8.o 6x9.o 6x10.o 6x12.o 6x13.o\
       7x13.o 7x14.o 8x13.o cursive_9x15.o dagger_9x14.o\
       inkblot_9x13.o radon_8x12.o radonWide_9x12.o\
       shadow_12x18.o shadowBold_12x18.o

ALL:$(TARGET)

# Rule to make library
$(TARGET): $(OBJS)
	$(AR) -rc $(TARGET) $(OBJS)
	$(RANLIB) $(TARGET)

clean:
	@echo "**** Cleaning..."
	@rm -f include/*~ include/*.bak
	@rm -f *.o *.O *.bak *~
spotless: clean
	@rm -f $(TARGET)
depend:
	makedepend -Y $(GCCINCDIR) *.c
# DO NOT DELETE
