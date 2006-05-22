// warning : this file is included by both arm and dsp code (thx zig, it's a lot cleaner like this :)

#define NES_BUFFER_WIDTH 272
#define NES_WIDTH 256
#define NES_PAL_HEIGHT 240
#define NES_NTSC_HEIGHT 224

#define LCD_WIDTH 224
#define LCD_HEIGHT 176

// offset of the area of our control variables in the dsp
// only 2 bytes space required, the rest will be dynamically set
#define DSP_CONTROL_OFFSET 0x90

#ifdef DSPCODE
# define DSP_RAM(a) ((volatile unsigned short *)(a))
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
#else
# define DSP_RAM(a) ((volatile unsigned short *)(0x40000 + (a)*2))
#endif

#define SDRAM_OFFSET 0x900000L

#define DSP_COM DSP_RAM(DSP_CONTROL_OFFSET)

typedef volatile struct {
  uint32 inBufAddr;
  uint32 outBufAddr;
  uint16 inBufReady;
  uint16 outBufReady;

  uint16 pal[32];
  uint16 lineOffset[NES_PAL_HEIGHT];

  uint16 hasDbgMsg;
  int16 dbgMsg[255];
} tDspCom;


extern tDspCom * dspCom;
