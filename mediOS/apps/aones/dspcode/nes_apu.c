/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is gm_free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the gm_free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the gm_free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** nes_apu.c
**
** NES APU emulation
** $Id$
*/

/*
** Changes for nester:
** changes are marked with DCR
**
** removed bad #includes
** nullified ASSERT and log_printf
** added custom log_printf()
** #included stlib.h for gm_malloc()
**
** - Darren Ranalli
*/

/*
** Changes for nester:
** support ExSounds
**
** - TAKEDA, toshiya
*/

//DCR#include "log.h"

#include "nes_apu.h"

/* GLI FIXES */

void nes6502_burn(int cycles){}

/* GLI END OF FIXES */

/* included for nes_irq() and nes_clearframeirq() */
#ifndef NSF_PLAYER
//DCR#include "nes.h"
#endif /* !NSF_PLAYER */

// DCR
#define ASSERT(CONDITION)
#define log_printf(STR)

#define  APU_OVERSAMPLE
#define  APU_VOLUME_DECAY(x)  ((x) -= ((x) >> 7))


/* pointer to active APU */
apu_t apu;

/* look up table madness */
static int32 decay_lut[16];
static long vbl_lut[32];
static long trilength_lut[128];

/* noise lookups for both modes */
#ifndef REALTIME_NOISE
#pragma DATA_SECTION(noise_long_lut, ".saram_d")
static int8 noise_long_lut[APU_NOISE_32K];
static int8 noise_short_lut[APU_NOISE_93];
#endif /* !REALTIME_NOISE */

/* vblank length table used for rectangles, triangle, noise */
static const uint8 vbl_length[32] =
{
    5, 127,
   10,   1,
   19,   2,
   40,   3,
   80,   4,
   30,   5,
    7,   6,
   13,   7,
    6,   8,
   12,   9,
   24,  10,
   48,  11,
   96,  12,
   36,  13,
    8,  14,
   16,  15
};

/* frequency limit of rectangle channels */
static const int freq_limit[8] =
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};

/* noise frequency lookup table */
static const int noise_freq[16] =
{
     4,    8,   16,   32,   64,   96,  128,  160,
   202,  254,  380,  508,  762, 1016, 2034, 4068
};

/* DMC transfer freqs */
const int dmc_clocks[16] =
{
   428, 380, 340, 320, 286, 254, 226, 214,
   190, 160, 142, 128, 106,  85,  72,  54
};

/* ratios of pos/neg pulse for rectangle waves */
static const int duty_lut[4] = { 2, 4, 8, 12 };

// for $4017:bit7 by T.Yano
static int apu_cnt_rate = 5;

void apu_setchan(long chan, boolean enabled)
{
   apu.mix_enable[chan] = enabled;
}

/* emulation of the 15-bit shift register the
** NES uses to generate pseudo-random series
** for the white noise channel
*/
#ifdef REALTIME_NOISE
INLINE int8 shift_register15(uint8 xor_tap)
{
   static int sreg = 0x4000;
   int bit0, tap, bit14;

   bit0 = sreg & 1;
   tap = (sreg & xor_tap) ? 1 : 0;
   bit14 = (bit0 ^ tap);
   sreg >>= 1;
   sreg |= (bit14 << 14);
   return (bit0 ^ 1);
}
#else /* !REALTIME_NOISE */
static void shift_register15(int8 *buf, int count)
{
   static int sreg = 0x4000;
   int bit0, bit1, bit6, bit14;

   if (count == APU_NOISE_93)
   {
      while (count--)
      {
         bit0 = sreg & 1;
         bit6 = (sreg & 0x40) >> 6;
         bit14 = (bit0 ^ bit6);
         sreg >>= 1;
         sreg |= (bit14 << 14);
         *buf++ = bit0 ^ 1;
      }
   }
   else /* 32K noise */
   {
      while (count--)
      {
         bit0 = sreg & 1;
         bit1 = (sreg & 2) >> 1;
         bit14 = (bit0 ^ bit1);
         sreg >>= 1;
         sreg |= (bit14 << 14);
         *buf++ = bit0 ^ 1;
      }
   }
}
#endif /* !REALTIME_NOISE */

/* RECTANGLE WAVE
** ==============
** reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle
** reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on
** reg2: 8 bits of freq
** reg3: 0-2=high freq, 7-4=vbl length counter
*/
#define  APU_RECTANGLE_OUTPUT (chan->output_vol)
static int32 apu_rectangle(rectangle_t *chan)
{
   int32 output;

#ifdef APU_OVERSAMPLE
   int num_times;
   int32 total;
#endif /* APU_OVERSAMPLE */

   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || chan->vbl_length <= 0)
      return APU_RECTANGLE_OUTPUT;

   /* vbl length counter */
   if (FALSE == chan->holdnote)
      chan->vbl_length -= apu_cnt_rate;

   /* envelope decay at a rate of (env_delay + 1) / 240 secs */
#if 1
   chan->env_phase -= 4 * apu_cnt_rate; /* 240/60 */
   while (chan->env_phase < 0)
   {
      chan->env_phase += chan->env_delay;

      if (chan->holdnote)
         chan->env_vol = (chan->env_vol + 1) & 0x0F;
      else if (chan->env_vol < 0x0F)
         chan->env_vol++;
   }
#else
   // speed hacks. Rick.
   {
	   long env_phase = chan->env_phase;
	   long env_delay = chan->env_delay;
	   long holdnote = chan->holdnote;
	   long env_vol = chan->env_vol;
	   env_phase -= 4 * apu_cnt_rate; /* 240/60 */
	   while (env_phase < 0)
	   {
		   env_phase += env_delay;
		   
		   if (holdnote)
			   env_vol = (env_vol + 1) & 0x0F;
		   else if (env_vol < 0x0F)
			   env_vol++;
	   }
	   chan->env_phase = env_phase;
	   chan->env_delay = env_delay;
	   chan->holdnote = holdnote;
	   chan->env_vol = env_vol;
   }
#endif

   /* TODO: using a table of max frequencies is not technically
   ** clean, but it is fast and (or should be) accurate 
   */
   if (chan->freq < 8 || (FALSE == chan->sweep_inc && chan->freq > chan->freq_limit))
      return APU_RECTANGLE_OUTPUT;

   /* frequency sweeping at a rate of (sweep_delay + 1) / 120 secs */
   if (chan->sweep_on && chan->sweep_shifts)
   {
      chan->sweep_phase -= 2 * apu_cnt_rate; /* 120/60 */
      while (chan->sweep_phase < 0)
      {
         chan->sweep_phase += chan->sweep_delay;

         if (chan->sweep_inc) /* ramp up */
         {
            if (TRUE == chan->sweep_complement)
               chan->freq += ~(chan->freq >> chan->sweep_shifts);
            else
               chan->freq -= (chan->freq >> chan->sweep_shifts);
         }
         else /* ramp down */
         {
            chan->freq += (chan->freq >> chan->sweep_shifts);
         }
      }
   }

   chan->phaseacc -= apu.cycle_rate; /* # of cycles per sample */
   if (chan->phaseacc >= 0)
      return APU_RECTANGLE_OUTPUT;

#ifdef APU_OVERSAMPLE
   num_times = total = 0;

   if (chan->fixed_envelope)
      output = chan->volume << 8; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F) << 8;
#endif /* APU_OVERSAMPLE */

   while (chan->phaseacc < 0)
   {
      chan->phaseacc += APU_TO_FIXED(chan->freq + 1);
      chan->adder = (chan->adder + 1) & 0x0F;

#ifdef APU_OVERSAMPLE
      if (chan->adder < chan->duty_flip)
         total += output;
      else
         total -= output;

      num_times++;
#endif /* APU_OVERSAMPLE */
   }

#ifdef APU_OVERSAMPLE
   chan->output_vol = total / num_times;
#else /* !APU_OVERSAMPLE */
   if (chan->fixed_envelope)
      output = chan->volume; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F);

   output<<=8;

   if (0 == chan->adder)
      chan->output_vol = output;
   else if (chan->adder == chan->duty_flip)
      chan->output_vol = -output;
#endif /* !APU_OVERSAMPLE */
   return APU_RECTANGLE_OUTPUT;
}

/* TRIANGLE WAVE
** =============
** reg0: 7=holdnote, 6-0=linear length counter
** reg2: low 8 bits of frequency
** reg3: 7-3=length counter, 2-0=high 3 bits of frequency
*/
//#define  APU_TRIANGLE_OUTPUT  (chan->output_vol + (chan->output_vol >> 1))
#define  APU_TRIANGLE_OUTPUT  ((chan->output_vol*21)>>4)
static int32 apu_triangle(triangle_t *chan)
{
   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || chan->vbl_length <= 0)
      return APU_TRIANGLE_OUTPUT;

   if (chan->counter_started)
   {
      if (chan->linear_length > 0)
         chan->linear_length -= 4 * apu_cnt_rate;  /* 240/60 */
      if (chan->vbl_length > 0 && FALSE == chan->holdnote)
         chan->vbl_length -= apu_cnt_rate;
   }
   else if (FALSE == chan->holdnote && chan->write_latency)
   {
      if (--chan->write_latency == 0)
         chan->counter_started = TRUE;
   }

   if (chan->linear_length <= 0 || chan->freq < APU_TO_FIXED(4)) /* inaudible */
      return APU_TRIANGLE_OUTPUT;

   chan->phaseacc -= apu.cycle_rate; /* # of cycles per sample */
   while (chan->phaseacc < 0)
   {
      chan->phaseacc += chan->freq;
      chan->adder = (chan->adder + 1) & 0x1F;

      if (chan->adder & 0x10)
         chan->output_vol -= (2 << 8);
      else
         chan->output_vol += (2 << 8);
   }

   return APU_TRIANGLE_OUTPUT;
}


/* WHITE NOISE CHANNEL
** ===================
** reg0: 0-3=volume, 4=envelope, 5=hold
** reg2: 7=small(93 byte) sample,3-0=freq lookup
** reg3: 7-4=vbl length counter
*/
//#define  APU_NOISE_OUTPUT  (chan->output_vol - (chan->output_vol >> 2))
#define  APU_NOISE_OUTPUT  ((chan->output_vol*13)>>4)

static int32 apu_noise(noise_t *chan)
{
   int32 outvol;

#if defined(APU_OVERSAMPLE) && defined(REALTIME_NOISE)
#else /* !(APU_OVERSAMPLE && REALTIME_NOISE) */
   int32 noise_bit;
#endif /* !(APU_OVERSAMPLE && REALTIME_NOISE) */
#ifdef APU_OVERSAMPLE
   int num_times;
   int32 total;
#endif /* APU_OVERSAMPLE */

   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || chan->vbl_length <= 0)
      return APU_NOISE_OUTPUT;

   /* vbl length counter */
   if (FALSE == chan->holdnote)
      chan->vbl_length -= apu_cnt_rate;

   /* envelope decay at a rate of (env_delay + 1) / 240 secs */
#if 1
   chan->env_phase -= 4 * apu_cnt_rate; /* 240/60 */
   while (chan->env_phase < 0)
   {
      chan->env_phase += chan->env_delay;

      if (chan->holdnote)
         chan->env_vol = (chan->env_vol + 1) & 0x0F;
      else if (chan->env_vol < 0x0F)
         chan->env_vol++;
   }
#else
   {
	   long env_phase = chan->env_phase;
	   long env_delay = chan->env_delay;
	   long holdnote = chan->holdnote;
	   long env_vol = chan->env_vol;
	   env_phase -= 4 * apu_cnt_rate; /* 240/60 */
	   while (env_phase < 0)
	   {
		   env_phase += env_delay;
		   
		   if (holdnote)
			   env_vol = (env_vol + 1) & 0x0F;
		   else if (env_vol < 0x0F)
			   env_vol++;
	   }
	   chan->env_phase = env_phase;
	   chan->env_delay = env_delay;
	   chan->holdnote = holdnote;
	   chan->env_vol = env_vol;
   }
#endif


   chan->phaseacc -= apu.cycle_rate; /* # of cycles per sample */
   if (chan->phaseacc >= 0)
      return APU_NOISE_OUTPUT;
   
#ifdef APU_OVERSAMPLE
   num_times = total = 0;

   if (chan->fixed_envelope)
      outvol = chan->volume; /* fixed volume */
   else
      outvol = (chan->env_vol ^ 0x0F);

   outvol<<=8;

#endif /* APU_OVERSAMPLE */

   while (chan->phaseacc < 0)
   {
      chan->phaseacc += chan->freq;

#ifdef REALTIME_NOISE

#ifdef APU_OVERSAMPLE
      if (shift_register15(chan->xor_tap))
         total += outvol;
      else
         total -= outvol;

      num_times++;
#else /* !APU_OVERSAMPLE */
      noise_bit = shift_register15(chan->xor_tap);
#endif /* !APU_OVERSAMPLE */

#else /* !REALTIME_NOISE */
      chan->cur_pos++;

      if (chan->short_sample)
      {
         if (APU_NOISE_93 == chan->cur_pos)
            chan->cur_pos = 0;
      }
      else
      {
         if (APU_NOISE_32K == chan->cur_pos)
            chan->cur_pos = 0;
      }

#ifdef APU_OVERSAMPLE
      if (chan->short_sample)
         noise_bit = noise_short_lut[chan->cur_pos];
      else
         noise_bit = noise_long_lut[chan->cur_pos];

      if (noise_bit)
         total += outvol;
      else
         total -= outvol;

      num_times++;
#endif /* APU_OVERSAMPLE */
#endif /* !REALTIME_NOISE */
   }

#ifdef APU_OVERSAMPLE
   chan->output_vol = total / num_times;
#else /* !APU_OVERSAMPLE */
   if (chan->fixed_envelope)
      outvol = chan->volume << 8; /* fixed volume */
   else
      outvol = (chan->env_vol ^ 0x0F) << 8;

#ifndef REALTIME_NOISE
   if (chan->short_sample)
      noise_bit = noise_short_lut[chan->cur_pos];
   else
      noise_bit = noise_long_lut[chan->cur_pos];
#endif /* !REALTIME_NOISE */

   if (noise_bit)
      chan->output_vol = outvol;
   else
      chan->output_vol = -outvol;
#endif /* !APU_OVERSAMPLE */

   return APU_NOISE_OUTPUT;
}


inline void apu_dmcreload(dmc_t *chan)
{
   chan->address = chan->cached_addr;
   chan->dma_length = chan->cached_dmalength;
   chan->irq_occurred = FALSE;

   //{char s[50];sprintf(s,"dmc a %x l %d",(uint16)chan->address,chan->dma_length);debug(s);}

   //gli: get sample data from arm 
   if(chan->address!=chan->prev_addr || chan->prev_dmalength<chan->dma_length){
      dspCom->sndDmcAddress=chan->address;
      dspCom->sndDmcLength=chan->dma_length/8;
      dspCom->sndDmcWantRead=1;
      armInt_trigger();
      while(dspCom->sndDmcWantRead);

      chan->prev_addr=chan->address;
      chan->prev_dmalength=chan->dma_length;
   }
}

/* DELTA MODULATION CHANNEL
** =========================
** reg0: 7=irq gen, 6=looping, 3-0=pointer to clock table
** reg1: output dc level, 6 bits unsigned
** reg2: 8 bits of 64-byte aligned address offset : $C000 + (value * 64)
** reg3: length, (value * 16) + 1
*/
//#define  APU_DMC_OUTPUT (chan->output_vol - (chan->output_vol >> 3))
#define  APU_DMC_OUTPUT ((chan->output_vol*13)>>4)
static int32 apu_dmc(dmc_t *chan)
{
   int delta_bit;

   //gli: what is this for?, it breaks my APU_WRE1 change, so let's remove it
   //APU_VOLUME_DECAY(chan->output_vol);

   /* only process when channel is alive */
   if (chan->dma_length)
   {
      chan->phaseacc -= apu.cycle_rate; /* # of cycles per sample */
      
      while (chan->phaseacc < 0)
      {
         chan->phaseacc += chan->freq;
         
         if (0 == (chan->dma_length & 7))
         {
            chan->cur_byte = dspCom->sndDmcData[chan->address-chan->cached_addr];
            
            chan->address++;
         }

         if (--chan->dma_length == 0)
         {
            /* if loop bit set, we're cool to retrigger sample */
            if (chan->looping)
               apu_dmcreload(chan);
            else
            {
               /* check to see if we should generate an irq */
               if (chan->irq_gen)
               {
                  chan->irq_occurred = TRUE;
#ifndef NSF_PLAYER
//DCR                  nes_irq();
#endif /* !NSF_PLAYER */
               }

               /* bodge for timestamp queue */
               chan->enabled = FALSE;
               break;
            }
         }
         delta_bit = (chan->dma_length & 7) ^ 7;

         /* positive delta */
         if (chan->cur_byte & (1 << delta_bit))
         {
            if (chan->regs[1] < 0x7D)
            {
               chan->regs[1] += 2;
               chan->output_vol += (2 << 8);
            }
         }
         /* negative delta */
         else            
         {
            if (chan->regs[1] > 1)
            {
               chan->regs[1] -= 2;
               chan->output_vol -= (2 << 8);
            }
         }
      }
   }
   return APU_DMC_OUTPUT;
}


void apu_regwrite(uint32 address, uint8 value)
{  
   int chan;

   switch (address)
   {
   /* rectangles */
   case APU_WRA0:
   case APU_WRB0:
      chan = (address & 4) ? 1 : 0;
      apu.rectangle[chan].regs[0] = value;

      apu.rectangle[chan].volume = value & 0x0F;
      apu.rectangle[chan].env_delay = decay_lut[value & 0x0F];
      apu.rectangle[chan].holdnote = (value & 0x20) ? TRUE : FALSE;
      apu.rectangle[chan].fixed_envelope = (value & 0x10) ? TRUE : FALSE;
      apu.rectangle[chan].duty_flip = duty_lut[value >> 6];
      break;

   case APU_WRA1:
   case APU_WRB1:
      chan = (address & 4) ? 1 : 0;
      apu.rectangle[chan].regs[1] = value;
      apu.rectangle[chan].sweep_on = (value & 0x80) ? TRUE : FALSE;
      apu.rectangle[chan].sweep_shifts = value & 7;
      apu.rectangle[chan].sweep_delay = decay_lut[(value >> 4) & 7];
      apu.rectangle[chan].sweep_inc = (value & 0x08) ? TRUE : FALSE;
      apu.rectangle[chan].freq_limit = freq_limit[value & 7];
      break;

   case APU_WRA2:
   case APU_WRB2:
      chan = (address & 4) ? 1 : 0;
      apu.rectangle[chan].regs[2] = value;
      apu.rectangle[chan].freq = (apu.rectangle[chan].freq & ~0xFF) | value;
      break;

   case APU_WRA3:
   case APU_WRB3:
      chan = (address & 4) ? 1 : 0;
      apu.rectangle[chan].regs[3] = value;

      apu.rectangle[chan].vbl_length = vbl_lut[value >> 3];
      apu.rectangle[chan].env_vol = 0;
      apu.rectangle[chan].freq = ((value & 7) << 8) | (apu.rectangle[chan].freq & 0xFF);
      apu.rectangle[chan].adder = 0;
      if ( apu.enable_reg & (1<<chan))
         apu.rectangle[chan].enabled = TRUE;
      break;

   /* triangle */
   case APU_WRC0:
      apu.triangle.regs[0] = value;
      apu.triangle.holdnote = (value & 0x80) ? TRUE : FALSE;

      if (FALSE == apu.triangle.counter_started && apu.triangle.vbl_length > 0)
         apu.triangle.linear_length = trilength_lut[value & 0x7F];
	   break;

   case APU_WRC2:
      apu.triangle.regs[1] = value;
      apu.triangle.freq = APU_TO_FIXED((((apu.triangle.regs[2] & 7) << 8) + value) + 1);
      break;

   case APU_WRC3:
      apu.triangle.regs[2] = value;
  
      /* this is somewhat of a hack.  there appears to be some latency on 
      ** the Real Thing between when trireg0 is written to and when the 
      ** linear length counter actually begins its countdown.  we want to 
      ** prevent the case where the program writes to the freq regs first, 
      ** then to reg 0, and the counter accidentally starts running because 
      ** of the sound queue's timestamp processing.
      **
      ** set latency to a couple hundred cycles -- should be plenty of time 
      ** for the 6502 code to do a couple of table dereferences and load up 
      ** the other triregs
      */

      /* 06/13/00 MPC -- seems to work OK */
      apu.triangle.write_latency = (int) (228 / APU_FROM_FIXED(apu.cycle_rate));

      apu.triangle.freq = APU_TO_FIXED((((value & 7) << 8) + apu.triangle.regs[1]) + 1);
      apu.triangle.vbl_length = vbl_lut[value >> 3];
      apu.triangle.counter_started = FALSE;
      apu.triangle.linear_length = trilength_lut[apu.triangle.regs[0] & 0x7F];
      if ( apu.enable_reg & 0x04) apu.triangle.enabled = TRUE;
      break;

   /* noise */
   case APU_WRD0:
      apu.noise.regs[0] = value;
      apu.noise.env_delay = decay_lut[value & 0x0F];
      apu.noise.holdnote = (value & 0x20) ? TRUE : FALSE;
      apu.noise.fixed_envelope = (value & 0x10) ? TRUE : FALSE;
      apu.noise.volume = value & 0x0F;
      break;

   case APU_WRD2:
      apu.noise.regs[1] = value;
      apu.noise.freq = APU_TO_FIXED(noise_freq[value & 0x0F]);

#ifdef REALTIME_NOISE
      apu.noise.xor_tap = (value & 0x80) ? 0x40: 0x02;
#else /* !REALTIME_NOISE */
      /* detect transition from long->short sample */
      if ((value & 0x80) && FALSE == apu.noise.short_sample)
      {
         /* recalculate short noise buffer */
         shift_register15(noise_short_lut, APU_NOISE_93);
         apu.noise.cur_pos = 0;
      }
      apu.noise.short_sample = (value & 0x80) ? TRUE : FALSE;
#endif /* !REALTIME_NOISE */
      break;

   case APU_WRD3:
      apu.noise.regs[2] = value;

      apu.noise.vbl_length = vbl_lut[value >> 3];
      apu.noise.env_vol = 0; /* reset envelope */
      if ( apu.enable_reg & 0x08) apu.noise.enabled = TRUE;
      break;

   /* DMC */
   case APU_WRE0:
	  apu.dmc.regs[0] = value;

      apu.dmc.freq = APU_TO_FIXED(dmc_clocks[value & 0x0F]);
      apu.dmc.looping = (value & 0x40) ? TRUE : FALSE;

      if (value & 0x80)
         apu.dmc.irq_gen = TRUE;
      else
      {
         apu.dmc.irq_gen = FALSE;
         apu.dmc.irq_occurred = FALSE;
      }
      break;

   case APU_WRE1: /* 7-bit DAC */
      value &= 0x7F; /* bit 7 ignored */
#if 0
      /* add the _delta_ between written value and
      ** current output level of the volume reg
      */
      apu.dmc.output_vol += ((value - apu.dmc.regs[1]) << 8);
#else
      //gli: no, do not add only delta, this fixes clicks at beginning of some samples & PCM
      apu.dmc.output_vol = value << 8;
#endif

      apu.dmc.regs[1] = value;
      break;

   case APU_WRE2:
      apu.dmc.regs[2] = value;
      apu.dmc.cached_addr = 0xC000 + (uint16) (value << 6);
      break;

   case APU_WRE3:
      apu.dmc.regs[3] = value;
      apu.dmc.cached_dmalength = ((value << 4) + 1) << 3;
      break;

   case APU_SMASK:
      /* bodge for timestamp queue */
      apu.dmc.enabled = (value & 0x10) ? TRUE : FALSE;

      apu.enable_reg = value;

      for (chan = 0; chan < 2; chan++)
      {
         if (0 == (value & (1 << chan)) )
         {
            apu.rectangle[chan].enabled = FALSE;
            apu.rectangle[chan].vbl_length = 0;
         }
      }

      if (0 == (value & 0x04))
      {
         apu.triangle.enabled = FALSE;
         apu.triangle.vbl_length = 0;
         apu.triangle.linear_length = 0;
         apu.triangle.counter_started = FALSE;
         apu.triangle.write_latency = 0;
      }

      if (0 == (value & 0x08))
      {
         apu.noise.enabled = FALSE;
         apu.noise.vbl_length = 0;
      }

      if (value & 0x10)
      {
         if (0 == apu.dmc.dma_length)
            apu_dmcreload(&apu.dmc);
      }
      else
      {
         apu.dmc.dma_length = 0;
         apu.dmc.irq_occurred = FALSE;
      }
      break;

      /* unused, but they get hit in some mem-clear loops */
   case 0x4009:
   case 0x400D:
      break;

   case 0x4017:
      if (value & 0x80)
         apu_cnt_rate = 4;
      else
         apu_cnt_rate = 5;
      break;

   default:
      break;
   }
}

/* Read from $4015 */
uint8 apu_read(uint32 address)
{
    uint8 value;

	value = 0;
#if 0
	/* Return 1 in 0-5 bit pos if a channel is playing */
	if (apu.rectangle[0].enabled_cur && apu.rectangle[0].vbl_length_cur>0)
	 value |= 0x01;
	if (apu.rectangle[1].enabled_cur && apu.rectangle[1].vbl_length_cur>0)
	 value |= 0x02;
	if (apu.triangle.enabled_cur && apu.triangle.vbl_length_cur>0)
	 value |= 0x04;
	if (apu.noise.enabled_cur && apu.noise.vbl_length_cur>0)
	 value |= 0x08;
	/* bodge for timestamp queue */
	if (apu.dmc.enabled_cur)
	 value |= 0x10;
	if (apu.dmc.irq_occurred_cur)
	 value |= 0x80;
#else
	/* Return 1 in 0-5 bit pos if a channel is playing */
	if (apu.rectangle[0].enabled && apu.rectangle[0].vbl_length>0)
	 value |= 0x01;
	if (apu.rectangle[1].enabled && apu.rectangle[1].vbl_length>0)
	 value |= 0x02;
	if (apu.triangle.enabled && apu.triangle.vbl_length>0)
	 value |= 0x04;
	if (apu.noise.enabled && apu.noise.vbl_length>0)
	 value |= 0x08;
	/* bodge for timestamp queue */
	if (apu.dmc.enabled)
	 value |= 0x10;
	if (apu.dmc.irq_occurred)
	 value |= 0x80;
#endif
    
    return value;
}

void apu_write_cur(uint32 address, uint8 value)
{
   /* for sync read $4015 */
   int chan;
   switch (address)
   {
   case APU_WRA0:
   case APU_WRB0:
      chan = (address & 4) ? 1 : 0;
      apu.rectangle[chan].holdnote_cur = (value & 0x20) ? TRUE : FALSE;
      break;
   case APU_WRA3:
   case APU_WRB3:
      chan = (address & 4) ? 1 : 0;
      apu.rectangle[chan].vbl_length_cur = vbl_length[value >> 3] * 5;
      if (apu.enable_reg_cur & (1<<chan))
         apu.rectangle[chan].enabled_cur = TRUE;
      break;
   case APU_WRC0:
      apu.triangle.holdnote_cur = (value & 0x80) ? TRUE : FALSE;
      break;
   case APU_WRC3:
      apu.triangle.vbl_length_cur = vbl_length[value >> 3] * 5;
      if (apu.enable_reg_cur & 0x04)
         apu.triangle.enabled_cur = TRUE;
      apu.triangle.counter_started_cur = TRUE;
      break;
   case APU_WRD0:
      apu.noise.holdnote_cur = (value & 0x20) ? TRUE : FALSE;
      break;
   case APU_WRD3:
      apu.noise.vbl_length_cur = vbl_length[value >> 3] * 5;
      if (apu.enable_reg_cur & 0x08)
         apu.noise.enabled_cur = TRUE;
      break;
   case APU_WRE0:
      apu.dmc.freq_cur = dmc_clocks[value & 0x0F];
      apu.dmc.phaseacc_cur = 0;
      apu.dmc.looping_cur = (value & 0x40) ? TRUE : FALSE;
      if (value & 0x80)
         apu.dmc.irq_gen_cur = TRUE;
      else
      {
         apu.dmc.irq_gen_cur = FALSE;
         apu.dmc.irq_occurred_cur = FALSE;
      }
      break;
   case APU_WRE3:
      apu.dmc.cached_dmalength_cur = (value << 4) + 1;
      break;
   case APU_SMASK:
      apu.enable_reg_cur = value;
      for (chan = 0; chan < 2; chan++)
      {
         if (0 == (value & (1 << chan)))
         {
            apu.rectangle[chan].enabled_cur = FALSE;
            apu.rectangle[chan].vbl_length_cur = 0;
         }
      }
      if (0 == (value & 0x04))
      {
         apu.triangle.enabled_cur = FALSE;
         apu.triangle.vbl_length_cur = 0;
         apu.triangle.counter_started_cur = FALSE;
      }
      if (0 == (value & 0x08))
      {
         apu.noise.enabled_cur = FALSE;
         apu.noise.vbl_length_cur = 0;
      }
      if (value & 0x10)
      {
         if(0 == apu.dmc.dma_length_cur)
         {
           apu.dmc.dma_length_cur = apu.dmc.cached_dmalength_cur;
         }
         apu.dmc.enabled_cur = TRUE;
      }
      else
      {
         apu.dmc.dma_length_cur = 0;
         apu.dmc.enabled_cur = FALSE;
         apu.dmc.irq_occurred_cur = FALSE;
      }
      break;
   }
}

void apu_getpcmdata(void **data, long *num_samples, long *sample_bits)
{
   *data = apu.buffer;
   *num_samples = apu.num_samples;
   *sample_bits = apu.sample_bits;
}


void apu_process(void *buf, int num_samples)
{
   int16 * buffer=buf;

   static int32 prev_sample = 0;
   int32 next_sample, accum;

   /* bleh */
   apu.buffer = buffer; 

   while (num_samples--)
   {

      //gli: handle reg writes queue
      while(dspCom->sndTail!=dspCom->sndHead){
	      volatile sndItem * item = &dspCom->sndQueue[dspCom->sndTail];
		  if (item->tick>apu.elapsed_cycles) break;
          apu_regwrite(item->address,item->value);
          dspCom->sndTail=(dspCom->sndTail+1)&SNDQUEUE_MASK;				    		
      }  

      apu.elapsed_cycles += APU_FROM_FIXED(apu.cycle_rate);
		
      accum = 0;
      if (apu.mix_enable[0]) accum += apu_rectangle(&apu.rectangle[0]);
      if (apu.mix_enable[1]) accum += apu_rectangle(&apu.rectangle[1]);
      if (apu.mix_enable[2]) accum += apu_triangle(&apu.triangle);
      if (apu.mix_enable[3]) accum += apu_noise(&apu.noise);
      if (apu.mix_enable[4]) accum += apu_dmc(&apu.dmc);
	
      /* do any *ing */
      if (APU_FILTER_NONE != apu.filter_type)
      {
         next_sample = accum;

         if (APU_FILTER_LOWPASS == apu.filter_type)
         {
            accum += prev_sample;
            accum >>= 1;
         }
         else
            next_sample =
            accum = (accum + accum + accum + prev_sample) >> 2;

         prev_sample = next_sample;
      }

      /* little extra kick for the kids */
      //accum <<= 1;

      /* prevent clipping */
      if (accum > 0x7FFFL)
         accum = 0x7FFFL;
      else if (accum < -0x8000L)
         accum = -0x8000L;
#if 0
      /* signed 16-bit output, unsigned 8-bit */
      if (16 == apu.sample_bits)
      {
		 *(buffer)++ = (int16) accum;
      }
      else
      {
	     *(buffer)++ = (accum >> 8);
      }
#else // 0
      *(buffer)++ = (int16) accum;
      *(buffer)++ = (int16) accum;
#endif // 0
   }

   /* resync cycle counter */
   //gli	
   apu.elapsed_cycles=dspCom->cpuCurCycle;
}

/* set the filter type */
void apu_setfilter(long filter_type)
{
   apu.filter_type = filter_type;
}

void apu_reset(void)
{
   long i;
   uint32 address;

   apu.elapsed_cycles = 0;

   // added by T.Yano
   // Reset rectangles
   for ( i=0; i<2; i++) {
      memset(apu.rectangle[i], 0, sizeof(apu.rectangle[i]));
   }
   apu.rectangle[0].sweep_complement = TRUE;
   apu.rectangle[1].sweep_complement = FALSE;
   memset(apu.triangle, 0, sizeof(apu.triangle));
   memset(apu.noise, 0, sizeof(apu.noise));
   memset(apu.dmc, 0, sizeof(apu.dmc));

   apu.enable_reg = 0;
   apu.enable_reg_cur = 0;

   /* use to avoid bugs =) */
   for (address = 0x4000; address <= 0x4013; address++)
   {
      apu_regwrite(address, 0);
      apu_write_cur(address, 0);
   }

#ifdef NSF_PLAYER
   apu_regwrite(0x400C, 0x10); /* silence noise channel on NSF start */
   apu_regwrite(0x4015, 0x0F);
   apu_write_cur(0x400c, 0x10);
   apu_write_cur(0x4015, 0x0F);
#else /* !NSF_PLAYER */
   apu_regwrite(0x4015, 0x00);
   apu_write_cur(0x4015, 0x00);
#endif /* !NSF_PLAYER */

   // for $4017:bit7 by T.Yano
   apu_cnt_rate = 5;
}

void apu_build_luts(long num_samples)
{
   long i;

   // decay_lut[], vbl_lut[], trilength_lut[] modified (x5) for $4017:bit7 by T.Yano
   /* lut used for enveloping and frequency sweeps */
   for (i = 0; i < 16; i++)
      decay_lut[i] = num_samples * (i + 1) * 5;

   /* used for note length, based on vblanks and size of audio buffer */
   for (i = 0; i < 32; i++)
      vbl_lut[i] = vbl_length[i] * num_samples * 5;

   /* triangle wave channel's linear length table */
   for (i = 0; i < 128; i++)
      trilength_lut[i] = num_samples * i * 5;

#ifndef REALTIME_NOISE
   /* generate noise samples */
   shift_register15(noise_long_lut, APU_NOISE_32K);
   shift_register15(noise_short_lut, APU_NOISE_93);
#endif /* !REALTIME_NOISE */
}

void apu_setparams(long sample_rate, long refresh_rate, long frag_size, long sample_bits)
{
   apu.sample_rate = sample_rate;
   apu.refresh_rate = refresh_rate;
   apu.sample_bits = sample_bits;

   apu.num_samples = sample_rate / refresh_rate;
   //apu.num_samples = frag_size;
   frag_size = frag_size; /* quell warnings */

   /* turn into fixed point! */
   apu.cycle_rate = (int32) (APU_BASEFREQ * 65536 / sample_rate);

   /* build various lookup tables for apu */
   apu_build_luts(apu.num_samples);

//DCR   apu_reset();
}

/* Initializes emulated sound hardware, creates waveforms/voices */
void apu_create(long sample_rate, long refresh_rate, long frag_size, long sample_bits)
{
   long channel;

   memset(apu, 0, sizeof(apu_t));	// Rick

   /* set the stupid flag to tell difference between two rectangles */
   apu.rectangle[0].sweep_complement = TRUE;
   apu.rectangle[1].sweep_complement = FALSE;

   /* set the update routine */
   apu.process = apu_process;

   apu_setparams(sample_rate, refresh_rate, frag_size, sample_bits);
            
   apu_reset(); //DCR
   
   for (channel = 0; channel < 6; channel++)
      apu_setchan(channel, TRUE);

   apu_setfilter(APU_FILTER_LOWPASS);
}

void apu_destroy(apu_t **src_apu)
{
   if (*src_apu)
   {
      free(*src_apu);
   }
}

void sync_apu_register()
{
   if (!apu.rectangle[0].holdnote_cur && apu.rectangle[0].vbl_length_cur > 0)
   {
      apu.rectangle[0].vbl_length_cur -= apu_cnt_rate;
   }
   if (!apu.rectangle[1].holdnote_cur && apu.rectangle[1].vbl_length_cur > 0)
   {
      apu.rectangle[1].vbl_length_cur -= apu_cnt_rate;
   }
   if (apu.triangle.counter_started_cur)
   {
      if (apu.triangle.vbl_length_cur > 0 && !apu.triangle.holdnote_cur)
      {
         apu.triangle.vbl_length_cur -= apu_cnt_rate;
      }
   }
   if (!apu.noise.holdnote_cur && apu.noise.vbl_length_cur > 0)
   {
      apu.noise.vbl_length_cur -= apu_cnt_rate;
   }
}

boolean sync_dmc_register(uint32 cpu_cycles)
{
	boolean irq_occurred = FALSE;

	// keep them local for speed. Rick.
	long phaseacc_cur = apu.dmc.phaseacc_cur;
	
	phaseacc_cur -= (int)cpu_cycles;
	
	if (phaseacc_cur < 0) {
		long freq_cur = apu.dmc.freq_cur * 8;
		long dma_length_cur = apu.dmc.dma_length_cur;
		long cached_dmalength_cur = apu.dmc.cached_dmalength_cur;
		long irq_occurred_cur = apu.dmc.irq_occurred_cur;
		long enabled_cur = apu.dmc.enabled_cur;
		long looping_cur = apu.dmc.looping_cur;
		long irq_gen_cur = apu.dmc.irq_gen_cur;
		
		while(phaseacc_cur < 0)
		{
			phaseacc_cur += freq_cur;
			if (dma_length_cur)
			{
				if (--dma_length_cur == 0)
				{
					if (looping_cur)
					{
						dma_length_cur = cached_dmalength_cur;
						irq_occurred_cur = FALSE;
					}
					else
					{
						dma_length_cur = 0;
						if (irq_gen_cur)
						{
							irq_occurred_cur = TRUE;
							irq_occurred = TRUE;
						}
						enabled_cur = FALSE;
					}
				}
			}
		}
		apu.dmc.dma_length_cur = dma_length_cur;
		apu.dmc.irq_occurred_cur = irq_occurred_cur;
		apu.dmc.enabled_cur = enabled_cur;
	}
	apu.dmc.phaseacc_cur = phaseacc_cur;

	return irq_occurred;
}
