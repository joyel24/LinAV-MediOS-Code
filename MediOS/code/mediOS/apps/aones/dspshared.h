#ifndef __DSPSHARED_H
#define __DSPSHARED_H

// warning : this file is included by both arm and dsp code (thx zig, it's a lot cleaner like this :)

//#define DSP_VID_PROFILE 

#define SAMPLE_RATE 32000

#define NES_BUFFER_WIDTH 272
#define NES_WIDTH 256
#define NES_PAL_HEIGHT 240
#define NES_NTSC_HEIGHT 224

#ifdef DSPCODE
#define LCD_WIDTH 224
#define LCD_HEIGHT 176
#endif

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

#define SNDQUEUE_LENGTH 256
#define SNDQUEUE_MASK (SNDQUEUE_LENGTH-1)

typedef struct {
  uint32 tick;
  uint16 address;
  uint16 value;
} sndItem;

typedef volatile struct {
  // init
  uint16 armInitFinished;
  uint16 dspInitFinished;
  uint16 chipNum;

  // video buffers addresses ans status
  uint32 inBufAddr;
  uint32 outBufAddr;
  uint16 inBufReady;
  uint16 outBufReady;

  // palette and line offsets
  uint16 pal[32];
  uint16 lineOffset[NES_PAL_HEIGHT];

  // sound filter
  uint16 sndFilter;

  // apu reset requests from arm
  uint16 sndWantApuReset;

  // sound pause requests from arm
  uint16 sndWantPause;
  uint16 sndIsPaused;

  // apu registers writes queue
  sndItem sndQueue[SNDQUEUE_LENGTH];
  uint16 sndHead;
  uint16 sndTail;
  
  // apu status
  uint16 sndStatusReg;

  // apu dmc samples requests to arm
  uint16 sndDmcWantRead;
  uint16 sndDmcAddress;
  uint16 sndDmcLength;
  uint16 sndDmcData[4096];

  // 6502 current cpu cycle
  uint32 cpuCurCycle;

  // debug messages
  uint16 hasDbgMsg;
  int16 dbgMsg[255];
} tDspCom;


extern tDspCom * dspCom;

#endif /*__DSPSHARED_H*/

