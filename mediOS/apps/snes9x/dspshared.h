// warning : this file is included by both arm and dsp code


// include profiling code
// slightly slower and takes some memory
#define PROFILE

// profile each emulated opcodes individually, very slow
//#define PROFILE_OPCODES

#ifdef PROFILE_OPCODES
# define PROFILE_ADDSZ 256
#else
# define PROFILE_ADDSZ 0
#endif

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

#define SDRAM_OFFSET 0x900000L

#define DSP_COM DSP_RAM(DSP_CONTROL_OFFSET)
#define DSP_FFT DSP_RAM(0x8000) // need to be 128 bytes aligned in dsp memory (cbrev function requirement)

//#define DSP_BUFFER_SIZE (4096*2)
#define DSP_PCM_SIZE (32768/2) // this buffer isn't anymore in dsp memory, but cannot be larger than 32768 for now
#define DSP_FFT_SIZE 1024
#define SPC_DSP_FIFO_SIZE 128  // this is the fifo where spc dsp commands are going through (spc dsp is emulated on arm for now)

typedef struct dsp_com {
  volatile uint16_t readPos;
  volatile uint16_t writePos;
  volatile uint16_t stop;
  //int16_t buffer[DSP_PCM_SIZE];
  uint32_t pcm_buffer_addr;

  volatile int16_t trigger_fft; // if 2 then fft data are ready, set to 1 to trigger a new fft calculation
  //int16_t fft[DSP_FFT_SIZE/2];
  uint16_t hello;
#ifdef PROFILE
  uint16_t prof[8+PROFILE_ADDSZ];
#endif

  volatile short spc_running; // 1: running (arm ask to), 0 : arm ask to stop, 2 : dsp has stopped
  uint32_t spc_ram_addr;
  uint32_t spc_xtraram_addr;
  uint32_t spc_dsp_addr;
  uint16_t spc_PC;
  uint16_t spc_A;
  uint16_t spc_X;
  uint16_t spc_Y;
  uint16_t spc_P;
  uint16_t spc_S;
  uint16_t spc_pages;
  uint16_t spc_setdsp;
  uint16_t spc_dsp_fifo[128];
  volatile uint16_t spc_dsp_fifo_read;
  volatile uint16_t spc_dsp_fifo_write;
  uint32_t spc_err_rate;
  uint16_t spc_channels;
  uint16_t spc_decodes;
  short spc_volumes[9][2];

  short dmairq;
  uint32_t spc_echo_addr;
} dsp_com_t;

extern dsp_com_t * dsp_com;

//#define FRQ (44100/4)
#define FRQ (32000)

#ifndef DSPCODE
void initDSP();
void write_dsp32(void * p, uint32_t value);
#endif
