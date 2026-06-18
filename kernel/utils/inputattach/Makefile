


RES = inputattach
RESOBJS = inputattach.o



all: $(RES)

$(RES): $(RESOBJS)
	$(CC) $(LDFLAGS) -o $@ $(RESOBJS)  $(LDLIBS$(LDLIBS_$@))



dummy_target:


clean:
	-rm -f $(SH) $(SHUTDOWN) $(REBOOT) *.elf *.gdb *.o

romfs:
	$(ROMFSINST) /bin/inputattach