// !!!!!! penser a remettre -o2 !!!!!!!!!

// VP petites remarques :
//
// sizeof(int) = 1 donc pareil que char et short : un mot de 16 bits
// sizeof(long) = 2 donc 32 bits
//
// ou sont passes les 64 bits ??
//

// do we resample in the aic irq (useful if pcm frequency is different from aic one)
#define INTERPOLATE

// use DMA for mcbsp transfert
#define USE_DMA

#include <stdio.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_pwr.h>
#include <csl_dma.h>
#include <csl_timer.h>
#include <Dsplib.h>

#include "../dspshared.h"

#include "mmu.h"
void SPC_update(unsigned char *buf);
short spc_running = 0;

dsp_com_t dsp_com_buffer;
dsp_com_t * dsp_com;

int init_aicdma();
void aicdma();

MCBSP_Config DataPortCfg= {
#ifdef USE_DMA
  0x0000,0x0200, /* SPCR : free running mode */
  0x00A0,0x00A1, /* RCR  : 32 bit receive data length */  
  0x00A0,0x00A0, /* XCR  : 32 bit transmit data length */            
  0x0000,0x3000, /* SRGR 1 & 2 */
  0x0000,0x0000, /* MCR  : single channel */
  // VP : frame transmit polarity was wrong (bit 3 has to be cleared)
  0x000E - 8,        /* PCR  : FSX, FSR active low, external FS/CLK source */
  0x0000,
  0x0000,
  0x0000,
  0x0000
#else
  0x0000,0x0200, /* SPCR : free running mode */
  0x00A0,0x0001, /* RCR  : 32 bit receive data length */  
  0x00A0,0x0000, /* XCR  : 32 bit transmit data length */            
  0x0000,0x3000,  
  0x0000,0x0000, /* MCR  : single channel */
  // VP : frame transmit polarity was wrong (bit 3 has to be cleared)
  (MCBSP_PCR_RMK(
       MCBSP_PCR_XIOEN_DEFAULT,      
       MCBSP_PCR_RIOEN_DEFAULT,      
       MCBSP_PCR_FSXM_DEFAULT,      
       MCBSP_PCR_FSRM_DEFAULT,       
       MCBSP_PCR_SCLKME_DEFAULT,       
       MCBSP_PCR_CLKXM_DEFAULT,       
       MCBSP_PCR_CLKRM_DEFAULT,      
       MCBSP_PCR_FSXP_DEFAULT,       
       MCBSP_PCR_FSRP_DEFAULT,       
       MCBSP_PCR_CLKXP_DEFAULT,      
       MCBSP_PCR_CLKRP_DEFAULT
       )&~0xf)|(0xe - 8),
  //0x000E - 8,        /* PCR  : FSX, FSR active low, external FS/CLK source */
  0x0000,
  0x0000,
  0x0000,
  0x0000
#endif
};     

void start_timer();

//Event id's
Uint16 xmtEventID;
Uint16 dmaEventID;

static MCBSP_Handle dataPort;

extern void VECSTART(void);

void prof(int id);
static uint16_t prof_timer;

interrupt void writeISR(void);

#pragma DATA_SECTION(fft_buf, ".fft")
short fft_buf[DSP_FFT_SIZE];
short * fft;

#define N_WAVE          1024		/* dimension of Sinewave[] */
static short Sinewave[257]; // TODO : optimize (could be only quarter of the size)
#define FIX_MPY(DEST,A,B) (DEST) = ((long)(A) * (long)(B))>>15

//#define HPIB(adr) (*( (volatile unsigned short * ) (0x10000 | adr) ))
#define HPIB(adr) REG16(adr)
char hello[8] = "world";
char hello2[8] = "world";
char world[8] = "hello";

/*      fix_window() - apply a Hanning window       */
void fix_window(short fr[], int n)
{
  int i, j, k;

  j = N_WAVE / n;
  n >>= 1;
  for (i = 0, k = N_WAVE / 4; i < n; ++i, k -= j)
    FIX_MPY(fr[i], fr[i], 16384 - (Sinewave[k] >> 1));
  n <<= 1;
  for (k -= j; i < n; ++i, k += j)
    FIX_MPY(fr[i], fr[i], 16384 - (Sinewave[k] >> 1));
}

// direction : 1 dsp-->sdram, 0 sdram-->dsp
int dma_dsp2sdram(void * dsp_addr, unsigned long sdram_addr, short length, short direction);
int dma_pending();
int test_and_set(int *, int value);

int dma_inuse;

static volatile int testdma = 0;
interrupt void testisr()
{
  testdma = 1;
}

void test_hpib()
{
  dma_dsp2sdram(hello2, 0xe10000L - 0x900000L, 12, 0);
  while (dma_pending());

  // test normal dsp dma too
#if 0
  {
    int event;
    DMA_Handle myhDma = DMA_open(DMA_CHA3, DMA_OPEN_RESET);

    /* Call DMA_config function to write our configuration    */
    /* values to DMA channel control registers                */
    /* In Example DMA1A, we defined a configuration structure */
    /* and called DMA_config function, passing the config     */
    /* structure as argument. In this example, DMA1B, we call */
    /* the DMA_configArgs function, passing each register     */
    /* value as an argument to the function.                  */
    
    DMA_configArgs(
		   myhDma,    
		   1 ,                                                 /* Priority */
		   DMA_DMMCR_RMK(
				 DMA_DMMCR_AUTOINIT_OFF,
				 DMA_DMMCR_DINM_ON,
				 DMA_DMMCR_IMOD_FULL_ONLY,
				 DMA_DMMCR_CTMOD_MULTIFRAME,
				 DMA_DMMCR_SLAXS_OFF,
				 DMA_DMMCR_SIND_POSTINC,
				 DMA_DMMCR_DMS_DATA,
				 DMA_DMMCR_DLAXS_OFF,
				 DMA_DMMCR_DIND_POSTINC,
				 DMA_DMMCR_DMD_DATA
				 ),                                                   /* DMMCR */
		   DMA_DMSFC_RMK(
				 DMA_DMSFC_DSYN_NONE,
				 DMA_DMSFC_DBLW_OFF,
				 DMA_DMSFC_FRAMECNT_OF(0)),                         /* DMSFC */
		   (DMA_AdrPtr)&world,                                    /* DMSRC */
		   (DMA_AdrPtr)&hello,                                    /* DMDST */
		   (Uint16)(8)                                        /* DMCTR */
		   );           
    
    DMA_FSET(DMPREC,INTOSEL,DMA_DMPREC_INTOSEL_CH2_CH3);
    IRQ_globalDisable();
    IRQ_setVecs((Uint32)(&VECSTART));
    event = DMA_getEventId(myhDma);
    IRQ_clear(event);
    IRQ_plug(event, &testisr);
    IRQ_enable(event);
    IRQ_globalEnable();

    /* Call DMA_start to begin the data transfer */
    DMA_start(myhDma);      
    
    /* Poll DMA status too see if its done       */                    
    //while(DMA_getStatus(myhDma));               
    while (!testdma);
    
    /* We are done, so close DMA channel */
    DMA_close(myhDma); 
  
  }
#endif


  dma_dsp2sdram(hello, 0xe10000L - 0x900000L, 12, 1);
  while (dma_pending());
}

void main(){
  int gie;
#define INC (0x10000*FRQ/32000) 
  //#define INC (0x10000*FRQ/44100) 
  
  *DSP_COM=0;

  CSL_init();
  
  test_hpib();

  dsp_com = &dsp_com_buffer;
  dsp_com->hello = (unsigned short) hello2;

  //test_dma();

  dataPort = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);
  
  if (dataPort==INV){
    return;
  }

  *DSP_COM = (unsigned short) dsp_com;

  // need DSP_FFT_SIZE alignement
  fft = (short *) (( (int) fft_buf+DSP_FFT_SIZE-1 ) & ~(DSP_FFT_SIZE-1) );

  gie=IRQ_globalDisable();
  IRQ_setVecs((Uint32)(&VECSTART));
#ifndef USE_DMA
  xmtEventID = MCBSP_getXmtEventId(dataPort);
  IRQ_clear(xmtEventID);
  IRQ_plug(xmtEventID, &writeISR);
  IRQ_enable(xmtEventID);
#endif

  MCBSP_config(dataPort,&DataPortCfg);
  
  MCBSP_start(dataPort,MCBSP_XMIT_START/*|MCBSP_SRGR_FRAMESYNC*/,0);

#ifdef USE_DMA
  init_aicdma();
#endif

  start_timer();

  IRQ_globalEnable();

  dsp_com->trigger_fft = 1;

  while(!dsp_com->stop) {
    static void refill();
    //refill();

    prof(0);
    //aicdma();

    if (dsp_com->spc_running == 1) {
      if (!spc_running) {
	mmu_reset();
	spc_running = 1;
      }
      prof(7);
      SPC_update(0);
    } else {
      spc_running = 0;
      dsp_com->spc_running = 2;      
      PWR_powerDown(PWR_CPU_PER_DOWN, PWR_WAKEUP_MI);
    }

    if (dsp_com->trigger_fft == 1) {
      int i, r;
      uint32_t addr = dsp_com->pcm_buffer_addr&-3;

      prof(1);
      r = dsp_com->readPos;
#if 1
      { static short rbuf[2][128];
      short * buf = rbuf[0];
      int cur_buf = 0;
      dma_inuse = 1;
      while (dma_pending());
      dma_dsp2sdram(rbuf[0], addr + ((long)r)*2, 256, 0);
      dma_inuse = 0;
      while (dma_pending());
      r = (r+128) & (DSP_PCM_SIZE-1);
      for (i=0; i<DSP_FFT_SIZE; /*i += 128/8*/) {
	long res;
	int j;
	//while (test_and_set(&dma_inuse, 1));
	if (i<DSP_FFT_SIZE-128/8) {
	  dma_inuse = 1;
	  while (dma_pending());
	  dma_dsp2sdram(rbuf[1-cur_buf], addr + ((long)r)*2, 256, 0);
	  dma_inuse = 0;
	}
	for (j=0; j<128; j+=8, i++) {
	  res = 
	    + ((long)buf[j+0]) + ((long)buf[j+1])
	    + ((long)buf[j+2]) + ((long)buf[j+3])
	    + ((long)buf[j+4]) + ((long)buf[j+5])
	    + ((long)buf[j+6]) + ((long)buf[j+7])
/* 	  + ((long)dsp_com->buffer[r+8]) + ((long)dsp_com->buffer[r+9]) */
/* 	  + ((long)dsp_com->buffer[r+10]) + ((long)dsp_com->buffer[r+11]) */
/* 	  + ((long)dsp_com->buffer[r+12]) + ((long)dsp_com->buffer[r+13]) */
/* 	  + ((long)dsp_com->buffer[r+14]) + ((long)dsp_com->buffer[r+15]) */
	  ;
	  fft[i] = res >> 3;
	}
	r = (r+128) & (DSP_PCM_SIZE-1);
	cur_buf = 1-cur_buf;
	buf = rbuf[cur_buf];
      }
      }
#endif

      prof(2);
      fix_window(fft, DSP_FFT_SIZE);
      prof(3);
      cbrev(fft, fft, DSP_FFT_SIZE/2);
      prof(4);
      rfft(fft, DSP_FFT_SIZE, 1);
      prof(5);
      for (i=0; i<DSP_FFT_SIZE/2; i++) {
	long r;
	//power(fft+i*2, &r, 2);
	//r >>= 8;
	r = ((long)fft[i*2])*((long)fft[i*2]) + ((long)fft[i*2+1])*((long)fft[i*2+1]) >>8;
	if (r>=0x8000) r = 0x7fff;
	fft[i] = r;
      }
      prof(6);
      sqrt_16(fft, fft, DSP_FFT_SIZE/2);
      dsp_com->trigger_fft = 3;
    }
  }

  *DSP_COM = 0;
}

static TIMER_Config TConfig = {
  TIMER_TCR_RMK(
    TIMER_TCR_SOFT_WAITZERO,
    TIMER_TCR_FREE_NOSOFT,
    TIMER_TCR_TRB_RESET,
    TIMER_TCR_TSS_START,
    TIMER_TCR_TDDR_OF(0)
  ),                        /* TCR0 */
  /*0x0800u*/0x8000         /* PRD0 */
};

interrupt void timerIsr()
{
#ifdef USE_DMA
  prof_timer++;
  aicdma();
#endif
}

static TIMER_Handle hTimer;
void start_timer()
{
  int eventId;

  hTimer = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);    
 
  /* Write configuration structure values to Timer control    */
  /* registers.                                               */
  TIMER_config(hTimer, &TConfig);  
 
  /* Get Event ID associated with Timer interrupt */ 
  eventId = TIMER_getEventId(hTimer);    

  /* Clear any pending Timer interrupts */
  IRQ_clear(eventId);                     

  /* Place interrupt service routine address at associated vector */
  IRQ_plug(eventId,timerIsr);

  /* Enable Timer interrupt */
  IRQ_enable(eventId);

  /* Start Timer */   
  TIMER_start(hTimer);  
}


interrupt void dummyISR(void)
{
}


static short l1 = 0, l2 = 0, r1 = 0, r2 = 0;
static long pos = 0;
static int pcm_cur_buf=0, pcm_filled_buf=0, pcm_pos=0;
#define PCMSZ 128
#pragma DATA_ALIGN(pcm_buffer, 4)
static short pcm_buffer[2][PCMSZ];
static short * pcm_ptr = pcm_buffer[0];

#ifdef USE_DMA

DMA_Handle hDma;
#else
void aicdma()
{
}
#endif

static void refill()
{
  if (!pcm_filled_buf && !dma_inuse/* && !dma_pending()*/) {
    uint16_t rpos = dsp_com->readPos;
    unsigned short avail = (dsp_com->writePos - rpos) & (DSP_PCM_SIZE-1);
    if (avail > PCMSZ) {
      while (dma_pending());
      dma_dsp2sdram(&pcm_buffer[pcm_cur_buf^1][0], (dsp_com->pcm_buffer_addr&-3) + rpos*2, 2*PCMSZ, 0);
      while (dma_pending());
      pcm_filled_buf = 1; // ready to be swaped
      // we do it here, when we're about certain the dma has completed
      dsp_com->readPos = (dsp_com->readPos + PCMSZ) & (DSP_PCM_SIZE-1);
    }
  }

  // swap pcm buffer when needed and if other buffer ready
  if (pcm_pos >= PCMSZ && pcm_filled_buf) {
    pcm_pos = 0;
    pcm_cur_buf ^= 1;
    pcm_filled_buf = 0;
    pcm_ptr = pcm_buffer[pcm_cur_buf];
  }
}

#ifdef USE_DMA
static int aicdma_inuse;
interrupt void dma_isr(void)
{
  DMA_stop(hDma);
  dsp_com->dmairq++;
  pcm_pos = PCMSZ;
  aicdma();
}

int init_aicdma()
{
/*   int i; */
/*   for (i=0; i<64; i++) */
/*     pcm_ptr[i] = 16000; */

  /* Open DMA channel 3 */
  hDma = DMA_open(DMA_CHA3, DMA_OPEN_RESET);    

  return 0;
}

void aicdma()
{
  if (aicdma_inuse) return;
  aicdma_inuse = 1;

  refill();
  if (!pcm_pos) {
    pcm_pos = 2;

    DMA_reset(hDma);
    /* Write configuration structure values to DMA control regs */ 
    DMA_configArgs(
		   hDma,    
		   0 ,                                                 /* Priority */
		   DMA_DMMCR_RMK(
				 DMA_DMMCR_AUTOINIT_OFF,
				 DMA_DMMCR_DINM_ON,
				 DMA_DMMCR_IMOD_FULL_ONLY,
				 0, /* not ABU */
				 DMA_DMMCR_SLAXS_OFF,
				 DMA_DMMCR_SIND_POSTINC,
				 DMA_DMMCR_DMS_DATA,
				 DMA_DMMCR_DLAXS_OFF,
				 DMA_DMMCR_DIND_NOMOD,
				 DMA_DMMCR_DMD_DATA
				 ),                                                   /* DMMCR */
		   DMA_DMSFC_RMK(
				 DMA_DMSFC_DSYN_XEVT0,
				 DMA_DMSFC_DBLW_ON,
				 DMA_DMSFC_FRAMECNT_OF(0)
				 ),                         /* DMSFC */ 
		   (DMA_AdrPtr)(pcm_ptr+2),                   /* DMSRC */
		   (DMA_AdrPtr)MCBSP_ADDR(DXR20),           /* DMDST */
		   (Uint16)(PCMSZ-2)/2-1                        /* DMCTR = buffsize */
		   );


    dmaEventID = DMA_getEventId(hDma);      
    
    /* Enable masking of DMA channel 3 interrupt in DMA */
    /* interrupt select register                        */
    DMA_FSET(DMPREC,INTOSEL,DMA_DMPREC_INTOSEL_CH2_CH3);
    
/*     DMA_FSET(DMPREC,FREE,1); */

    IRQ_globalDisable();
    IRQ_disable(dmaEventID);
    IRQ_clear(dmaEventID); 
    IRQ_plug(dmaEventID,&dma_isr);
    IRQ_enable(dmaEventID);              
    IRQ_globalEnable();

    /* Set value of the DMA index register, DMIXD0 */
    //DMA_RSET(DMIDX0,1);
    
    while(!(MCBSP_xrdy(dataPort))){
      ;
    }
/*     l1 = pcm_ptr[0]; */
/*     r1 = pcm_ptr[1]; */
/*     MCBSP_write32(dataPort,((long)l1<<16) | r1); */
    MCBSP_write32(dataPort,*(long *)pcm_ptr);

    /* Start DMA transfer */
    DMA_start(hDma);

  }

  aicdma_inuse = 0;
}
#endif

#ifndef INTERPOLATE
interrupt void writeISR(void)
{
  prof_timer++;

  refill();
    
  MCBSP_write32(dataPort, (((unsigned long)l2)<<16)|r2);
  
  if (pcm_pos < PCMSZ) {
    l2 = pcm_ptr[pcm_pos++];
    r2 = pcm_ptr[pcm_pos++];
  }
}
#else
interrupt void writeISR(void)
{
  unsigned long l, r;
  long ipos = 0x10000L - pos;

  prof_timer++;

  refill();
    
  l = (((long)l1)*((long)ipos) + ((long)l2)*((long)pos));
  r = (((long)r1)*((long)ipos) + ((long)r2)*((long)pos));
  r = (l&0xffff0000) | (r>>16);
  MCBSP_write32(dataPort, r);
  
  if (pcm_pos < PCMSZ) {
    pos += INC;
    while (pos >= 0x10000) {
      l1 = l2;
      r1 = r2;
      l2 = pcm_ptr[pcm_pos];
      r2 = pcm_ptr[pcm_pos+1];
      pos -= 0x10000;
      pcm_pos += 2;
    }
  }
}
#endif

/* static short l1 = 0, l2 = 0, r1 = 0, r2 = 0; */
/* static long pos = 0; */
/* interrupt void writeISR(void) */
/* { */
/*   unsigned long l, r; */
/*   long ipos = 0x10000L - pos; */
/*   uint16_t rpos = dsp_com->readPos; */
    
/*   l = (((long)l1)*((long)ipos) + ((long)l2)*((long)pos)); */
/*   r = (((long)r1)*((long)ipos) + ((long)r2)*((long)pos)); */
/*   r = (l&0xffff0000) | (r>>16); */
/*   MCBSP_write32(dataPort, r); */
    
/*   if (rpos != dsp_com->writePos) { */
/*     pos += INC; */
/*     while (pos >= 0x10000) { */
/*       l1 = l2; */
/*       r1 = r2; */
/*       l2 = dsp_com->buffer[rpos]; */
/*       r2 = dsp_com->buffer[rpos+1]; */
/*       pos -= 0x10000; */
/*       dsp_com->readPos = (rpos + 2) & (DSP_PCM_SIZE-1); */
/*     } */
/*   } */
/* } */

int max_id, cur_id;
void prof(int id)
{
  static uint16_t last;
  uint16_t a = prof_timer;
  dsp_com->prof[cur_id] += a - last;

  last = a;
  cur_id = id;
  if (max_id < id) max_id = id;
}

static short Sinewave[257] = {
  0, 201, 402, 603, 804, 1005, 1206, 1406,
  1607, 1808, 2009, 2209, 2410, 2610, 2811, 3011,
  3211, 3411, 3611, 3811, 4011, 4210, 4409, 4608,
  4807, 5006, 5205, 5403, 5601, 5799, 5997, 6195,
  6392, 6589, 6786, 6982, 7179, 7375, 7571, 7766,
  7961, 8156, 8351, 8545, 8739, 8932, 9126, 9319,
  9511, 9703, 9895, 10087, 10278, 10469, 10659, 10849,
  11038, 11227, 11416, 11604, 11792, 11980, 12166, 12353,
  12539, 12724, 12909, 13094, 13278, 13462, 13645, 13827,
  14009, 14191, 14372, 14552, 14732, 14911, 15090, 15268,
  15446, 15623, 15799, 15975, 16150, 16325, 16499, 16672,
  16845, 17017, 17189, 17360, 17530, 17699, 17868, 18036,
  18204, 18371, 18537, 18702, 18867, 19031, 19194, 19357,
  19519, 19680, 19840, 20000, 20159, 20317, 20474, 20631,
  20787, 20942, 21096, 21249, 21402, 21554, 21705, 21855,
  22004, 22153, 22301, 22448, 22594, 22739, 22883, 23027,
  23169, 23311, 23452, 23592, 23731, 23869, 24006, 24143,
  24278, 24413, 24546, 24679, 24811, 24942, 25072, 25201,
  25329, 25456, 25582, 25707, 25831, 25954, 26077, 26198,
  26318, 26437, 26556, 26673, 26789, 26905, 27019, 27132,
  27244, 27355, 27466, 27575, 27683, 27790, 27896, 28001,
  28105, 28208, 28309, 28410, 28510, 28608, 28706, 28802,
  28897, 28992, 29085, 29177, 29268, 29358, 29446, 29534,
  29621, 29706, 29790, 29873, 29955, 30036, 30116, 30195,
  30272, 30349, 30424, 30498, 30571, 30643, 30713, 30783,
  30851, 30918, 30984, 31049,
  31113, 31175, 31236, 31297,
  31356, 31413, 31470, 31525, 31580, 31633, 31684, 31735,
  31785, 31833, 31880, 31926, 31970, 32014, 32056, 32097,
  32137, 32176, 32213, 32249, 32284, 32318, 32350, 32382,
  32412, 32441, 32468, 32495, 32520, 32544, 32567, 32588,
  32609, 32628, 32646, 32662, 32678, 32692, 32705, 32717,
  32727, 32736, 32744, 32751, 32757, 32761, 32764, 32766,
  32767, /*32766, 32764, 32761, 32757, 32751, 32744, 32736,
  32727, 32717, 32705, 32692, 32678, 32662, 32646, 32628,
  32609, 32588, 32567, 32544, 32520, 32495, 32468, 32441,
  32412, 32382, 32350, 32318, 32284, 32249, 32213, 32176,
  32137, 32097, 32056, 32014, 31970, 31926, 31880, 31833,
  31785, 31735, 31684, 31633, 31580, 31525, 31470, 31413,
  31356, 31297, 31236, 31175, 31113, 31049, 30984, 30918,
  30851, 30783, 30713, 30643, 30571, 30498, 30424, 30349,
  30272, 30195, 30116, 30036, 29955, 29873, 29790, 29706,
  29621, 29534, 29446, 29358, 29268, 29177, 29085, 28992,
  28897, 28802, 28706, 28608, 28510, 28410, 28309, 28208,
  28105, 28001, 27896, 27790, 27683, 27575, 27466, 27355,
  27244, 27132, 27019, 26905, 26789, 26673, 26556, 26437,
  26318, 26198, 26077, 25954, 25831, 25707, 25582, 25456,
  25329, 25201, 25072, 24942, 24811, 24679, 24546, 24413,
  24278, 24143, 24006, 23869, 23731, 23592, 23452, 23311,
  23169, 23027, 22883, 22739, 22594, 22448, 22301, 22153,
  22004, 21855, 21705, 21554, 21402, 21249, 21096, 20942,
  20787, 20631, 20474, 20317, 20159, 20000, 19840, 19680,
  19519, 19357, 19194, 19031, 18867, 18702, 18537, 18371,
  18204, 18036, 17868, 17699, 17530, 17360, 17189, 17017,
  16845, 16672, 16499, 16325, 16150, 15975, 15799, 15623,
  15446, 15268, 15090, 14911, 14732, 14552, 14372, 14191,
  14009, 13827, 13645, 13462, 13278, 13094, 12909, 12724,
  12539, 12353, 12166, 11980, 11792, 11604, 11416, 11227,
  11038, 10849, 10659, 10469, 10278, 10087, 9895, 9703,
  9511, 9319, 9126, 8932, 8739, 8545, 8351, 8156,
  7961, 7766, 7571, 7375, 7179, 6982, 6786, 6589,
  6392, 6195, 5997, 5799, 5601, 5403, 5205, 5006,
  4807, 4608, 4409, 4210, 4011, 3811, 3611, 3411,
  3211, 3011, 2811, 2610, 2410, 2209, 2009, 1808,
  1607, 1406, 1206, 1005, 804, 603, 402, 201,*/
};

