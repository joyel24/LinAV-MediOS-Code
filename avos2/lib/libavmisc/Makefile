# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavmisc.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include

# State the object files that the main target is made up of
OBJS = debug.o math.O string.O system.O

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

debug.o: $(BUILDDIR)/lib/gcc/arm-linux-uclibc/$(BUILDVER)/include/stdarg.h
debug.o: $(BUILDDIR)/include/string.h
debug.o: $(BUILDDIR)/include/features.h
debug.o: $(BUILDDIR)/include/sys/cdefs.h
debug.o: $(BUILDDIR)/include/bits/uClibc_config.h
debug.o: $(BUILDDIR)/lib/gcc/arm-linux-uclibc/$(BUILDVER)/include/stddef.h
debug.o: $(BUILDDIR)/include/stdlib.h
debug.o: $(BUILDDIR)/include/sys/types.h
debug.o: $(BUILDDIR)/include/bits/types.h
debug.o: $(BUILDDIR)/include/bits/kernel_types.h
debug.o: $(BUILDDIR)/include/bits/pthreadtypes.h
debug.o: $(BUILDDIR)/include/bits/sched.h
debug.o: $(BUILDDIR)/include/time.h
debug.o: $(BUILDDIR)/include/endian.h
debug.o: $(BUILDDIR)/include/bits/endian.h
debug.o: $(BUILDDIR)/include/sys/select.h
debug.o: $(BUILDDIR)/include/bits/select.h
debug.o: $(BUILDDIR)/include/bits/sigset.h
debug.o: $(BUILDDIR)/include/bits/time.h
debug.o: $(BUILDDIR)/include/sys/sysmacros.h
debug.o: $(BUILDDIR)/include/alloca.h
debug.o: ../../include/uart.h
