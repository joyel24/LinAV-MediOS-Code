# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavata.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include 

# State the object files that the main target is made up of
OBJS = ata.O ata.o

all:$(TARGET)

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

ata.o: $(BUILDDIR)/include/stdlib.h
ata.o: $(BUILDDIR)/include/features.h
ata.o: $(BUILDDIR)/include/sys/cdefs.h
ata.o: $(BUILDDIR)/include/bits/uClibc_config.h
ata.o: $(BUILDDIR)/include/linux/stddef.h
ata.o: $(BUILDDIR)/include/sys/types.h
ata.o: $(BUILDDIR)/include/bits/types.h
ata.o: $(BUILDDIR)/include/bits/kernel_types.h
ata.o: $(BUILDDIR)/include/bits/pthreadtypes.h
ata.o: $(BUILDDIR)/include/bits/sched.h
ata.o: $(BUILDDIR)/include/time.h
ata.o: $(BUILDDIR)/include/endian.h
ata.o: $(BUILDDIR)/include/bits/endian.h
ata.o: $(BUILDDIR)/include/sys/select.h
ata.o: $(BUILDDIR)/include/bits/select.h
ata.o: $(BUILDDIR)/include/bits/sigset.h
ata.o: $(BUILDDIR)/include/bits/time.h
ata.o: $(BUILDDIR)/include/sys/sysmacros.h
ata.o: $(BUILDDIR)/include/alloca.h
ata.o: $(BUILDDIR)/include/string.h
ata.o: ../../include/ata.h ../../include/debug.h
