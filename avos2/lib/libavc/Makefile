# Include global definitions
include ../../Makefile.conf

# State the main target to build
TARGET = ../libavc.a

#LOCAL GCCFLAGS
GCCINCDIRS = -I../../include -I../../include/std

# State the object files that the main target is made up of
OBJS = avmalloc.o dirent.o fat.o file.o

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

avmalloc.o: ../../include/std/assert.h ../../include/std/features.h
avmalloc.o: ../../include/std/sys/cdefs.h
avmalloc.o: $(BUILDDIR)/include/bits/uClibc_config.h
avmalloc.o: ../../include/std/memory.h ../../include/std/string.h
avmalloc.o: ../../include/std/stddef.h ../../include/avmalloc.h
dirent.o: ../../include/std/string.h ../../include/std/features.h
dirent.o: ../../include/std/sys/cdefs.h
dirent.o: $(BUILDDIR)/include/bits/uClibc_config.h
dirent.o: ../../include/std/stddef.h ../../include/std/stdlib.h
dirent.o: ../../include/std/sys/types.h ../../include/std/bits/types.h
dirent.o: $(BUILDDIR)/include/bits/kernel_types.h
dirent.o: $(BUILDDIR)/include/bits/pthreadtypes.h
dirent.o: $(BUILDDIR)/include/bits/sched.h
dirent.o: ../../include/std/time.h ../../include/std/endian.h
dirent.o: ../../include/std/bits/endian.h ../../include/std/sys/select.h
dirent.o: ../../include/std/bits/select.h ../../include/std/bits/sigset.h
dirent.o: ../../include/std/bits/time.h ../../include/std/sys/sysmacros.h
dirent.o: ../../include/std/alloca.h ../../include/dirent.h
dirent.o: ../../include/std/stdbool.h ../../include/fat.h ../../include/ata.h
dirent.o: ../../include/debug.h
fat.o: ../../include/std/string.h ../../include/std/features.h
fat.o: ../../include/std/sys/cdefs.h
fat.o: $(BUILDDIR)/include/bits/uClibc_config.h
fat.o: ../../include/std/stddef.h ../../include/ata.h ../../include/fat.h
fat.o: ../../include/std/stdbool.h ../../include/dirent.h
fat.o: ../../include/debug.h
file.o: ../../include/std/stdlib.h ../../include/std/features.h
file.o: ../../include/std/sys/cdefs.h
file.o: $(BUILDDIR)/include/bits/uClibc_config.h
file.o: ../../include/std/stddef.h ../../include/std/sys/types.h
file.o: ../../include/std/bits/types.h
file.o: $(BUILDDIR)/include/bits/kernel_types.h
file.o: $(BUILDDIR)/include/bits/pthreadtypes.h
file.o: $(BUILDDIR)/include/bits/sched.h
file.o: ../../include/std/time.h ../../include/std/endian.h
file.o: ../../include/std/bits/endian.h ../../include/std/sys/select.h
file.o: ../../include/std/bits/select.h ../../include/std/bits/sigset.h
file.o: ../../include/std/bits/time.h ../../include/std/sys/sysmacros.h
file.o: ../../include/std/alloca.h ../../include/std/string.h
file.o: ../../include/file.h ../../include/std/stdbool.h ../../include/fat.h
file.o: ../../include/dirent.h ../../include/ata.h ../../include/debug.h
