# Include global definitions
include ../Makefile.conf

# State the main target to build
TARGET = boot.bin

#LOCAL GCCFLAGS
GCCFLAGS = -static-libgcc
GCCINCDIRS = -I../include

#LOCAL LDFLAGS
LDFLAGS = -L../lib -lavrtc -lavc -lavhardware -lavgraphics -lavfonts -lavmisc -lavata -lc -lfloat -lgcc


# State the object files that the main target is made up of
OBJS = bootstart.O boot.o

ALL:$(TARGET)

# Rule to make executable
$(TARGET): $(OBJS)
	$(MAKE) -C plugins
	$(LD) -N -Ttext 0x03400000 -e main2 -o $(TARGET).elf $(OBJS) $(LDFLAG)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)
	@$(RM) -f $(TARGET).elf

clean:
	$(MAKE) -C plugins clean
	@echo "**** Cleaning..."
	@rm -f include/*~ include/*.bak
	@rm -f *.o *.O *.bak *~
spotless: clean
	@rm -f $(TARGET)

depend:
	makedepend -Y $(GCCINCDIR) *.c
# DO NOT DELETE

