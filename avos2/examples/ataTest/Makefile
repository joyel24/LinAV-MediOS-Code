# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ataTest.bin

#LOCAL GCCFLAGS
GCCFLAGS = -static-libgcc
GCCINCDIRS = -I../../include

#LOCAL LDFLAGS
LDFLAGS = -L../../lib -lavhardware -lavgraphics -lavfonts -lavmisc -lavata -lc -lfloat -lgcc


# State the object files that the main target is made up of
OBJS = ataTest.o

ALL:$(TARGET)

# Rule to make executable
$(TARGET): $(OBJS)
	$(LD) -N -Ttext 0x03000000 -e main -o $(TARGET).elf $(OBJS) $(LDFLAG)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)
	@$(RM) -f $(TARGET).elf

clean:
	@echo "**** Cleaning..."
	@rm -f include/*~ include/*.bak
	@rm -f *.o *.bak *~
spotless: clean
	@rm -f $(TARGET)

depend:
	makedepend -Y $(GCCINCDIR) *.c
# DO NOT DELETE

ataTest.o: ../../include/ata.h ../../include/uart.h
ataTest.o: $(BUILDDIR)/arm-linux/include/string.h
ataTest.o: $(BUILDDIR)/arm-linux/include/features.h
ataTest.o: $(BUILDDIR)/arm-linux/include/sys/cdefs.h
ataTest.o: $(BUILDDIR)/arm-linux/include/bits/uClibc_config.h
ataTest.o: $(BUILDDIR)/lib/gcc-lib/arm-linux-uclibc/$(BUILDVER)/include/stddef.h
ataTest.o: ../../include/graphics.h ../../include/osdDSC25.h
ataTest.o: ../../include/buttons.h ../../include/fonts.h ../../include/usb.h
ataTest.o: ../../include/system.h
