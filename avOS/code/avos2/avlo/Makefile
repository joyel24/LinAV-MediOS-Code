# Include global definitions
include ../Makefile.conf

# State the main target to build
TARGET = avlo.bin

#LOCAL GCCFLAGS
GCCFLAGS = -static-libgcc
GCCINCDIRS = -I../include

#LOCAL LDFLAGS
LDFLAGS = -L../lib -lavc -lavmisc -lavhardware -lavgraphics -lavfonts  -lavata -lc -lfloat -lgcc


# State the object files that the main target is made up of
OBJS = bootstart.O avlo.o parse_cfg.o


ALL:$(TARGET)

# Rule to make executable
$(TARGET): $(OBJS)
	$(LD) -N -Ttext 0x03400000 -e main2 -o $(TARGET).elf $(OBJS) $(LDFLAG)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET)
	@$(RM) -f $(TARGET).elf

clean:
	@echo "**** Cleaning..."
	@rm -f include/*~ include/*.bak
	@rm -f *.o *.O *.bak *~ *.bin *.head *.ajz
spotless: clean
	@rm -f $(TARGET)

depend:
	makedepend -Y $(GCCINCDIR) *.c
# DO NOT DELETE

