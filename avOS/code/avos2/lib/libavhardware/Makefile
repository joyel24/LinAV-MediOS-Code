# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavhardware.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include

# State the object files that the main target is made up of
OBJS = buttons.O cpld.O dma.O ecr.O gio.O\
       i2c.O interrupts.O\
       power.O timers.O uart.O usb.O

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
