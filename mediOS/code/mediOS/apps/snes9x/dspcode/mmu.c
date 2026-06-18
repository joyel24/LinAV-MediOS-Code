#include <string.h>

#include <csl_pwr.h>

#include "../dspshared.h"
#define String toto
#include "snes9x.h"
#undef String
#include "spc700.h"
#include "memmap.h"
// #include "display.h"
// #include "cpuexec.h"
#include "apu.h"
#include "soundux.h"
uint8 S9xAPUGetByteZ (uint8 address);
uint8 S9xAPUGetByte (uint16 address);
void S9xAPUSetByteZ (uint8, uint8 address);
void S9xAPUSetByte (uint8, uint16 address);
#include "mmu.h"

#include "dsp.h"

void aicdma();

// direction : 1 dsp-->sdram, 0 sdram-->dsp
int dma_dsp2sdram(void * dsp_addr, unsigned long sdram_addr, short length, short direction);
int dma_pending();
extern int dma_inuse;
int test_and_set(int *, int value);

short timers[3];

#define PSZS  7 // 128 bytes per page
#define PSZ   (1<<PSZS)
#define NBPAGES 32 // works well with less (like 32) too

#pragma DATA_SECTION(pages, ".sintab")
u16 pages[NBPAGES][PSZ/2];

//#pragma DATA_SECTION(loaded, ".sintab")
// page size 128 --> 2Kb
u16 loaded[65536/PSZ];

// complete it when needed
u16 used[NBPAGES];
u16 count[NBPAGES];
u16 flags[NBPAGES];

static uint32_t sdaddr;


// spc emulation rate
#define RATE 800
static long samples_per_mix;


void mmu_reset()
{
  int i;

  DSP_Reset();

  for (i=0; i<NBPAGES; i++)
    used[i] = 65536/PSZ;
  for (i=0; i<65536/PSZ; i++)
    loaded[i] = NBPAGES;
  sdaddr = dsp_com->spc_ram_addr & -3;

  dma_inuse = 1;
  while (dma_pending());
  // load extra ram
  dma_dsp2sdram(APU.ExtraRAM, dsp_com->spc_xtraram_addr&-3, 64, 0);
  while (dma_pending());
  // load spc dsp ram
  dma_dsp2sdram(APU.DSP, dsp_com->spc_dsp_addr&-3, 128, 0);
  while (dma_pending());
  dma_inuse = 0;

  // unpack extraram
  for (i=31; i>=0; i--) {
    APU.ExtraRAM[i*2+1] = APU.ExtraRAM[i] >>8;
    APU.ExtraRAM[i*2] = APU.ExtraRAM[i]&0xff;
  }

  // unpack dsp ram
  for (i=63; i>=0; i--) {
    APU.DSP[i*2+1] = APU.DSP[i] >>8;
    APU.DSP[i*2] = APU.DSP[i]&0xff;
  }


  // from here, need to be moved 

  S9xResetSound (TRUE);
  //S9xSetEchoEnable (0);

  APU.ShowROM = TRUE;

  IAPU.OneCycle = 1;
  IAPU.TwoCycles = IAPU.OneCycle * 2;


  APURegisters.PC = dsp_com->spc_PC;
  APURegisters.YA.B.A = dsp_com->spc_A;
  APURegisters.X = dsp_com->spc_X;
  APURegisters.YA.B.Y = dsp_com->spc_Y;
  APURegisters.P = dsp_com->spc_P;
  APURegisters.S = dsp_com->spc_S;

  timers[0] = GET(0xfd);
  timers[1] = GET(0xfe);
  timers[2] = GET(0xff);

  for (i = 0; i < 4; i ++)
    {
      APU.OutPorts[i] = GET(0xf4 + i);
    }
  IAPU.TimerErrorCounter = 0;

  for (i = 0; i < 3; i ++)
    {
      if (GET(0xfa + i) == 0)
	APU.TimerTarget[i] = 0x100;
      else
	APU.TimerTarget[i] = GET(0xfa + i);
    }

  IAPU.PC = APURegisters.PC;
  //IAPU.RAM = 0;

  S9xAPUUnpackStatus ();
  if (APUCheckDirectPage ())
    IAPU.DirectPage = /*IAPU.RAM + */0x100;
  else
    IAPU.DirectPage = 0; //IAPU.RAM;

  S9xSetAPUControl (GET(0xf1));



  // sound init
  Settings.DisableSampleCaching = TRUE; // VP
  Settings.APUEnabled = TRUE;
  Settings.InterpolatedSound = TRUE; // VP
  Settings.SoundEnvelopeHeightReading = TRUE;
  Settings.DisableSoundEcho = FALSE; // VP
  Settings.EnableExtraNoise = FALSE; // VP

  samples_per_mix = FRQ / RATE * 2;
  so.playback_rate = FRQ;
  so.err_rate = dsp_com->spc_err_rate;
  so.err_rate = (uint32)(SNES_SCANLINE_TIME * 0x10000UL / (1.0 / (double) so.playback_rate));

  so.buffer_size *= 2;
  so.sixteen_bit = TRUE;

#if 1    
  S9xSetEchoDelay(APU.DSP [APU_EDL] & 0xf);
/*   for (i = 0; i < 8; i++) */
/*     S9xSetSoundFrequency(i, SoundData.channels [i].hertz); */
  so.buffer_size = samples_per_mix;
  so.stereo = TRUE;

  so.encoded = FALSE;
  so.mute_sound = FALSE;

  S9xSetSoundMute (FALSE);
  IAPU.APUExecuting = TRUE;

  S9xFixSoundAfterSnapshotLoad ();

  S9xSetFrequencyModulationEnable (APU.DSP[APU_PMON]);
  S9xSetMasterVolume (APU.DSP[APU_MVOL_LEFT], APU.DSP[APU_MVOL_RIGHT]);
  S9xSetEchoVolume (APU.DSP[APU_EVOL_LEFT], APU.DSP[APU_EVOL_RIGHT]);
  S9xSetSoundControl(255); // turn all channels on

  {
    uint8 mask = 1;
    int type;
    APU.KeyedChannels = 0;
    for (i = 0; i < 8; i++, mask <<= 1) {
        S9xFixEnvelope (i,
                        APU.DSP[APU_GAIN + (i << 4)],
                        APU.DSP[APU_ADSR1 + (i << 4)],
                        APU.DSP[APU_ADSR2 + (i << 4)]);
/* 	APU.DSP[APU_VOL_LEFT + (i << 4)] /= 2; */
/* 	APU.DSP[APU_VOL_RIGHT + (i << 4)] /= 2; */
        S9xSetSoundVolume (i,
                           APU.DSP[APU_VOL_LEFT + (i << 4)],
                           APU.DSP[APU_VOL_RIGHT + (i << 4)]);
        if (APU.DSP [APU_NON] & mask)
            type = SOUND_NOISE;
        else
            type = SOUND_SAMPLE;
        S9xSetSoundType (i, type);
	S9xSetSoundHertz (i, (long) ((APU.DSP[APU_P_LOW + (i << 4)]
				      + (APU.DSP[APU_P_HIGH + (i << 4)] << 8))
				     & FREQUENCY_MASK)/* * 8*/);
        if ((APU.DSP[APU_KON] & mask) != 0)
	{
            APU.KeyedChannels |= mask;
            S9xPlaySample (i);
	}
    }
  }
#endif
}

static u16 mmu_load(u16 page)
{
  u16 min = 0xffff;
  int i, mini = 0;
  for (i=0; i<NBPAGES; i++) {
    if (used[i] == 65536/PSZ) {
      mini = i;
      break;
    }
    if (count[i] < min) {
      min = count[i];
      mini = i;
    }
    count[i] = 0; // TODO only sometimes ? 
  }
  //count[mini] = 0;

  dma_inuse = 1;
  if (used[mini] != 65536/PSZ) {
    if (flags[mini]) {
      while (dma_pending());
      // save previous page
      dma_dsp2sdram(pages[mini], sdaddr + used[mini]*PSZ, PSZ, 1);
    }
    
    loaded[used[mini]] = NBPAGES; // mark it as out
  }

  // setup new page
  used[mini] = page;
  loaded[page] = mini;
  flags[mini] = 0; // reset all flags

  while (dma_pending());
  // load new page
  dma_dsp2sdram(pages[mini], sdaddr + page*PSZ, PSZ, 0);
  while (dma_pending());
  dma_inuse = 0;

  dsp_com->spc_pages++;

  return mini;
}

u16 mmu_getbyte(u16 addr)
{
  u16 page = addr >>PSZS;
  u16 p = loaded[page];

  if (p == NBPAGES) p = mmu_load(page);
  
  count[p]++;

  if (addr&1) {
    return pages[p][(addr&(PSZ-1))>>1] >>8;
  } else {
    return pages[p][(addr&(PSZ-1))>>1] & 0xff;
  }
}

void mmu_getbytes(u16 addr, int sz, short * restrict ptr)
{
  while (sz > 0) {
    u16 page;
    u16 p;
    int n, i;
    
    page = addr >>PSZS;
    p = loaded[page];
    if (p == NBPAGES) p = mmu_load(page);
    
    count[p]++;

    i = addr&(PSZ-1);
    n = ( i+sz <= PSZ)? sz : PSZ-i;
    sz -= n;
    addr += n;
    if (i&1) {
      *ptr++ = pages[p][(i++)>>1] >>8;
      n--;
    }
    i >>= 1;
    while (n > 1) {
      u16 a;
      a = pages[p][i++];
      *ptr++ = a&0xff;
      *ptr++ = a>>8;
      n -= 2;
    }
    if (n>0) {
      *ptr++ = pages[p][i]&0xff;
    }
  }
}

void mmu_setbytes(u16 addr, int sz, const short * restrict ptr)
{
  while (sz > 0) {
    u16 page;
    u16 p;
    int n, i;
    
    page = addr >>PSZS;
    p = loaded[page];
    if (p == NBPAGES) p = mmu_load(page);
    
    count[p]++;
    flags[p] = 1; // mark page as modified

    i = addr&(PSZ-1);
    n = ( i+sz <= PSZ)? sz : PSZ-i;
    sz -= n;
    addr += n;
    if (i&1) {
      pages[p][i>>1] = (pages[p][i>>1]&0xff) | (*ptr++ << 8);
      i++;
      n--;
    }
    i >>= 1;
    while (n > 1) {
      short a = *ptr++;
      pages[p][i++] = (a&0xff) | (*ptr++ <<8);
      n -= 2;
    }
    if (n>0) {
      pages[p][i] = (pages[p][i]&0xff00) | (*ptr++&0xff);
    }
  }
}

void mmu_setbyte(u16 addr, u16 value)
{
  u16 page = addr >>PSZS;
  u16 p = loaded[page];
  u16 a = (addr&(PSZ-1))>>1;

  if (p == NBPAGES) p = mmu_load(page);

  count[p]++;
  flags[p] = 1; // mark page as modified
  
  if (addr&1) {
    pages[p][a] = (pages[p][a]&0xff) | (value << 8);
  } else {
    pages[p][a] = (pages[p][a]&0xff00) | (value&0xff);
  }
}

#if 0
static void DoTimer (void)
{
  APURegisters.PC = IAPU.PC/* - IAPU.RAM*/;

  if (APU.TimerEnabled [2]) {
      APU.Timer [2] ++;
      if (APU.Timer [2] >= APU.TimerTarget [2]) {
	SET(0xff, (GET(0xff) + 1) & 0xf);
 	  APU.Timer [2] = 0;
	}
    }
  if (IAPU.TimerErrorCounter >= 8)
    {
      IAPU.TimerErrorCounter = 0;
      if (APU.TimerEnabled [0])
	{
	  APU.Timer [0]++;
	  if (APU.Timer [0] >= APU.TimerTarget [0])
	    {
	      SET(0xfd, (GET(0xfd) + 1) & 0xf);
	      APU.Timer [0] = 0;

	    }
	}
      if (APU.TimerEnabled [1])
	{
	  APU.Timer [1]++;
	  if (APU.Timer [1] >= APU.TimerTarget [1])
	    {
	      SET(0xfe, (GET(0xfe) + 1) & 0xf);
	      APU.Timer [1] = 0;
	    }
	}
    }
}
#endif

//int SPC_slowdown_cycle_shift = 4;
#define SPC_slowdown_cycle_shift 4
int SPC_slowdown_instructions = 80;
int SPC_slowdown_instructions2 = 5;
extern uint8 OP0;
extern uint8 OP1;
extern uint8 OP2;

int fifo_write(int * buf, int count)
{
  int ini = count;

  while ( count > 0 ) {
    int count2, count3;
    int w = dsp_com->writePos;

    count2 = (dsp_com->readPos - w - 2) & (DSP_PCM_SIZE-1);
    if (!count2) {
      PWR_powerDown(PWR_CPU_DOWN, PWR_WAKEUP_MI); // With PWR_CPU_PER_DOWN, timer doesn't run anymore
      //aicdma();
      continue;
    }

    count2 >>= 1;
    if (count2 > count) 
      count2 = count;
    count -= count2;

    dma_inuse = 1;
    while (dma_pending());

    if (w+count2*2 > DSP_PCM_SIZE)
      count3 = (DSP_PCM_SIZE - w)/2;
    else
      count3 = count2;
    dma_dsp2sdram(buf, (dsp_com->pcm_buffer_addr&-3) + w*2, count3*4, 1);
    buf += count3*2;
    w = (w+count3*2)&(DSP_PCM_SIZE-1);
    count2 -= count3;
    if (count2>0) {
      while (dma_pending());
      dma_dsp2sdram(buf, (dsp_com->pcm_buffer_addr&-3) + w*2, count2*4, 1);
      buf += count2*2;
      w = (w+count2*2)&(DSP_PCM_SIZE-1);
    }
    dma_inuse=0;

    dsp_com->writePos = w;
  }
  return ini - count;
}


void SPC_update(unsigned char *buf)
{
  // APU_LOOP
  int c, ic, oc;
/*   int opc = IAPU.PC+256; */

  prof(7);

  /* VP : rewrote this loop, it was completely wrong in original
     spcxmms-0.2.1 */
  for (c = 0; c < 2048000 / RATE; ) {
    oc = c;
    for (ic = c + 32; c < ic; ) {
      int cycles;
/*       static int toto; */
/*       OP0 = toto++ & 0xff; */
      //dsp_com->spc_PC = IAPU.PC;
      //mmu_getbytes(IAPU.PC, 3, &OP0);
      OP0 = GET(IAPU.PC);
/*       OP1 = GET(IAPU.PC+1); */
/*       OP2 = GET(IAPU.PC+2); */
#if 0
      // slower :(
      switch (IAPU.PC - opc) {
      case 0:
	break;
      case 1:
	OP0 = OP1;
	OP1 = OP2;
	OP2 = GET(IAPU.PC+2);
	break;
      case -1:
	OP1 = OP0;
	OP2 = OP1;
	OP0 = GET(IAPU.PC);
	break;
      case 2:
	OP0 = OP2;
	OP1 = GET(IAPU.PC+1);
	OP2 = GET(IAPU.PC+2);
	break;
      case -2:
	OP2 = OP0;
	OP0 = GET(IAPU.PC);
	OP1 = GET(IAPU.PC+1);
	break;
      default:
	OP0 = GET(IAPU.PC);
	OP1 = GET(IAPU.PC+1);
	OP2 = GET(IAPU.PC+2);
      }
      opc = IAPU.PC;
#endif
      cycles = S9xAPUCycleLengths [OP0];

      {
	// BIG TRACE
/* 	queue_dsp(0x8200); // trace block command */
/* 	S9xAPUPackStatus(); */
/* 	queue_dsp(IAPU.PC); */
/* 	queue_dsp(APURegisters.YA.B.A); */
/* 	queue_dsp(APURegisters.YA.B.Y); */
/* 	queue_dsp(APURegisters.X); */
/* 	queue_dsp( (APURegisters.P<<8) | APURegisters.S ); */

	// in case there are remaining 8 bits errors in spc700.cpp these are safety nets
/* 	APURegisters.P &= 0xff; */
/* 	APURegisters.S &= 0xff; */
/* 	APURegisters.YA.B.A &= 0xff;   // <---- the important one */
/* 	APURegisters.YA.B.Y &= 0xff; */
/* 	IAPU._Zero &= 0xff; */
/* 	IAPU._Carry &= 1; */
      }

#ifdef PROFILE_OPCODES
      //      prof(8+OP0);
#endif
      (*S9xApuOpcodes[OP0]) ();
      //APU_EXECUTE1();

      if (IAPU.Slowdown > 0) {
	/* VP : in slowdown mode, SPC executes 2^SPC_slowdown_cycle_shift 
	   times slower */
	c += cycles << SPC_slowdown_cycle_shift;
	IAPU.Slowdown--;
      } else {
	/* VP : no slowdown, normal speed of the SPC */
	c += cycles;
      }
    }

#ifdef PROFILE_OPCODES
    //prof(7);
#endif
    /* VP : Execute timer required number of times */
/*     for (ic = 0; ic < (c / 32) - (oc / 32) ; ic++) { */
/*       IAPU.TimerErrorCounter ++; */
/*       DoTimer(); */
/*     } */
    {
      int n = (c / 32) - (oc / 32);
      
      APURegisters.PC = IAPU.PC/* - IAPU.RAM*/;

      if (APU.TimerEnabled [2]) {
	APU.Timer [2] += n;
	while (APU.Timer [2] >= APU.TimerTarget [2]) {
	  timers[2] = (timers[2]+1)&0xff;
/* 	  SET(0xff, (GET(0xff) + 1) & 0xf); */
 	  APU.Timer [2] -= APU.TimerTarget [2];
	}
      }
      if (APU.TimerEnabled [0])
	{
	  APU.Timer [0]+=n;
	  while (APU.Timer [0] >= 8*APU.TimerTarget [0])
	    {
	      timers[0] = (timers[0]+1)&0xff;
/* 	      SET(0xfd, (GET(0xfd) + 1) & 0xf); */
	      APU.Timer [0] -= 8*APU.TimerTarget [0];
	      
	    }
	}
      if (APU.TimerEnabled [1])
	{
	  APU.Timer [1]+=n;
	  while (APU.Timer [1] >= 8*APU.TimerTarget [1])
	    {
	      timers[1] = (timers[1]+1)&0xff;
/* 	      SET(0xfe, (GET(0xfe) + 1) & 0xf); */
	      APU.Timer [1] -= 8*APU.TimerTarget [1];
	    }
	}
    }

  }

  dsp_com->spc_PC = IAPU.PC;

  if (1) {
    static uint16 buf[128]; // can it be smaller ?
    int i;

    prof(6);
    S9xMixSamples ((uint8 *)buf, samples_per_mix);
    //DSP_Update(buf, samples_per_mix/2);

    for (i=0; i<8; i++) {
      dsp_com->spc_volumes[i][0] = SoundData.channels[i].left_vol_level;
      dsp_com->spc_volumes[i][1] = SoundData.channels[i].right_vol_level;
    }
    dsp_com->spc_volumes[8][0] = SoundData.master_volume[0];
    dsp_com->spc_volumes[8][1] = SoundData.master_volume[1];

    prof(5);
    fifo_write((int *)buf, samples_per_mix/2);
/*     queue_dsp(0x8300); // start sample block */
/*     for (i=0; i<samples_per_mix; i++) */
/*       queue_dsp(buf[i]); */
    return;
  }

  queue_dsp(0x8100); // mark end of frame

}

//static int dspfifopos;
extern int cur_id;
void queue_dsp(u16 value)
{
  dsp_com->spc_setdsp++;

#ifdef PROFILE
  if ( ((dsp_com->spc_dsp_fifo_read - dsp_com->spc_dsp_fifo_write - 1) & (SPC_DSP_FIFO_SIZE-1)) == 0) {
    int save_prof = cur_id;
    prof(1);
#endif
    while ( ((dsp_com->spc_dsp_fifo_read - dsp_com->spc_dsp_fifo_write - 1) & (SPC_DSP_FIFO_SIZE-1)) == 0);
#ifdef PROFILE
    prof(save_prof);
  }
#endif

  dsp_com->spc_dsp_fifo[dsp_com->spc_dsp_fifo_write] = value;
  dsp_com->spc_dsp_fifo_write = (dsp_com->spc_dsp_fifo_write+1)&(SPC_DSP_FIFO_SIZE-1);

/*   if (dspfifopos >= sizeof(dsp_com->spc_dsp_fifo)) { */
/*     dspfifopos = 0; */
/*     dsp_com->spc_dsp_fifo_ready = 1; */
/*     prof(0); */
/*     while (dsp_com->spc_dsp_fifo_ready); */
/*     prof(7); */
/*   } */
/*   dsp_com->spc_dsp_fifo[dspfifopos++] = value; */
}
