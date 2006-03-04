// warning : this file is included by both arm and dsp code

// offset of the area of our control variables in the dsp
// only 2 bytes space required, the rest will be dynamically set
#define DSP_CONTROL_OFFSET 0x90
//#define DSP_CONTROL_OFFSET 0x5800
//#define DSP_CONTROL_OFFSET 0x8000

#ifdef DSPCODE
# define DSP_RAM(a) ((volatile unsigned short *)(a))
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;
#else
# define DSP_RAM(a) ((volatile unsigned short *)(0x40000 + (a)*2))
#endif

#define DSP_COM DSP_RAM(DSP_CONTROL_OFFSET)

#define DSP_BUFFER_SIZE (4096*4)

typedef struct dsp_com {
  volatile uint16_t readPos;
  volatile uint16_t writePos;
  volatile uint16_t stop;
  uint16_t buffer[DSP_BUFFER_SIZE];
} dsp_com_t;

extern dsp_com_t * dsp_com;

//#define FRQ (44100/4)
#define FRQ (32000)

#ifndef DSPCODE
void initDSP();
#endif
