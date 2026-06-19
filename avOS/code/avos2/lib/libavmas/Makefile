# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavmas.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include

# State the object files that the main target is made up of
OBJS = mas.O mas.o

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

mas.o: ../../include/mas.h
