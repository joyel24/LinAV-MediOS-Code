# !!!!!!! PENSER A REMETTRE -o2 !!!!!!!!!!!!!

#TIPATH =	E:/gmini/C/CCStudio_v3.1_EVAL
TIPATH =	F:/CCStudio_v3.1_EVAL
#TIPATH =	/mnt/gigadaube1/CCStudio_v3.1_EVAL
#TIPATH =	/home/zig/wine/C/CCStudio_v3.1_EVAL
WINETIPATH =	$(TIPATH)
#WINETIPATH =	I:$(TIPATH)

CL =		$(WINETIPATH)/c5400/cgtools/bin/cl500.exe #
STRIP =		$(WINETIPATH)/c5400/cgtools/bin/strip500.exe #
#CL =		wine $(WINETIPATH)/c5400/cgtools/bin/cl500.exe #

#override PATH:=$(PATH):$(TIPATH)\bin:$(TIPATH)\cc\bin:$(TIPATH)\c2000\cgtools\bin:$(TIPATH)\c2400\cgtools\bin:$(TIPATH)\c6000\cgtools\bin:$(TIPATH)\c6000\evm6x\bin:$(TIPATH)\c5400\cgtools\bin:$(TIPATH)\c5500\cgtools\bin:$(TIPATH)\TMS470\cgtools\bin:$(TIPATH)\plugins\bios:$(TIPATH)\bin\utilities\tconf


#OPTFLAGS =	-O2 -ms -op3 -on2 
OPTFLAGS =	-O3 -oi256 -op3 -mn -on2 -pm
CFLAGS =	$(OPTFLAGS) -DDSPCODE -d"CHIP_5409" -c -k -ss \
		-I"$(WINETIPATH)/C5400/xdais/include" \
		-I"$(WINETIPATH)/C5400/rtdx/include" \
		-I"$(WINETIPATH)/C5400/csl/include" \
		-I"$(WINETIPATH)/C5400/bios/include" \
		-I"$(WINETIPATH)/C5400/dsplib/include" \
		-I"$(WINETIPATH)/C5400/cgtools/include" \
		-DHOST_LITTLE_ENDIAN -DSPC_PLAYER=1 -DNO_INLINE_SET_GET

LDFLAGS =	$(OPTFLAGS) -z -c -m"./test.map" -w -x \
		-i"$(WINETIPATH)/C5400/xdais/lib" \
		-i"$(WINETIPATH)/C5400/rtdx/lib" \
		-i"$(WINETIPATH)/C5400/csl/lib" \
		-i"$(WINETIPATH)/C5400/bios/lib" \
		-i"$(WINETIPATH)/C5400/cgtools/lib" \
		"./lnk.cmd"

#		"-lrts.lib" \
#		"-lcsl5409.lib"

LIBS =		"$(WINETIPATH)/C5400/cgtools/lib/rts.lib" \
		"$(WINETIPATH)/C5400/csl/lib/csl5409.lib" \
		"$(WINETIPATH)/C5400/dsplib/54xdsp.lib"


OBJ =		snes9x_dsp.obj vectors.obj portaccess.obj spc700.obj globals.obj apu.obj mmu.obj soundux.obj dsp.obj
#OBJ =		dummy.obj snes9x_dsp.obj vectors.obj
#OBJ =		snes9x_dsp.obj sn76496.obj vectors.obj

TARGET =	snes9x_dsp.out

all:	$(TARGET)

$(TARGET): $(OBJ)
	$(CL) $(LDFLAGS) $(OBJ) $(LIBS) -o "$@"
	$(STRIP) $@

clean:
	rm -f *.obj *.out

snes9x_dsp.obj: ../dspshared.h

.SUFFIXES: .c .asm .obj
.c.obj:
	$(CL) $(CFLAGS) "$<"

.cpp.obj:
	$(CL) $(CFLAGS) "$<"

.asm.obj:
	$(CL) $(CFLAGS) "$<"
