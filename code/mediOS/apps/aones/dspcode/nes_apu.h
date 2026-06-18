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
** nes_apu.h
**
** NES APU emulation header file
** $Id$
*/

#ifndef _NES_APU_H_
#define _NES_APU_H_

#include <stdio.h>
#include <csl.h>

#include "../dspshared.h"

/* GLI FIXES */

typedef short boolean;
#define INLINE inline

/* GLI END OF FIXES */

/* define this for realtime generated noise */
//#define  REALTIME_NOISE

#define  APU_WRA0       0x4000
#define  APU_WRA1       0x4001
#define  APU_WRA2       0x4002
#define  APU_WRA3       0x4003
#define  APU_WRB0       0x4004
#define  APU_WRB1       0x4005
#define  APU_WRB2       0x4006
#define  APU_WRB3       0x4007
#define  APU_WRC0       0x4008
#define  APU_WRC2       0x400A
#define  APU_WRC3       0x400B
#define  APU_WRD0       0x400C
#define  APU_WRD2       0x400E
#define  APU_WRD3       0x400F
#define  APU_WRE0       0x4010
#define  APU_WRE1       0x4011
#define  APU_WRE2       0x4012
#define  APU_WRE3       0x4013

#define  APU_SMASK      0x4015

/* length of generated noise */
#define  APU_NOISE_32K  0x3FFF
#define  APU_NOISE_93   93

//#define  APU_BASEFREQ   1789772.7272727272727272
#define  APU_BASEFREQ   1789772.5

/* to/from 16.16 fixed point */
#if 0
#define  APU_TO_FIXED(x)    ((x) << 16)
#define  APU_FROM_FIXED(x)  ((x) >> 16)
#else
#define  APU_TO_FIXED(x)    ((x) * 0x10000)
#define  APU_FROM_FIXED(x)  ((x) / 0x10000)
#endif
// ----------------------------------------------------------------------------
// APU Sound struct

/* channel structures */
/* As much data as possible is precalculated,
** to keep the sample processing as lean as possible
*/
 
typedef struct rectangle_s
{
   uint8 regs[4];

   boolean enabled;
   
   int32 phaseacc;
   int32 freq;
   int32 output_vol;
   boolean fixed_envelope;
   boolean holdnote;
   uint8 volume;

   int32 sweep_phase;
   int32 sweep_delay;
   boolean sweep_on;
   uint8 sweep_shifts;
   uint8 sweep_length;
   boolean sweep_inc;

   /* this may not be necessary in the future */
   int32 freq_limit;

   /* rectangle 0 uses a complement addition for sweep
   ** increases, while rectangle 1 uses subtraction
   */
   boolean sweep_complement;

   int32 env_phase;
   int32 env_delay;
   uint8 env_vol;

   long vbl_length;
   uint8 adder;
   int duty_flip;

   /* for sync read $4105 */
   boolean enabled_cur;
   boolean holdnote_cur;
   long vbl_length_cur;
} rectangle_t;

typedef struct triangle_s
{
   uint8 regs[3];

   boolean enabled;

   int32 freq;
   int32 phaseacc;
   int32 output_vol;

   uint8 adder;

   boolean holdnote;
   boolean counter_started;
   /* quasi-hack */
   int write_latency;

   long vbl_length;
   long linear_length;

   /* for sync read $4105 */
   boolean enabled_cur;
   boolean holdnote_cur;
   boolean counter_started_cur;
   long vbl_length_cur;

} triangle_t;

typedef struct noise_s
{
   uint8 regs[3];

   boolean enabled;

   int32 freq;
   int32 phaseacc;
   int32 output_vol;

   int32 env_phase;
   int32 env_delay;
   uint8 env_vol;
   boolean fixed_envelope;
   boolean holdnote;

   uint8 volume;

   long vbl_length;

#ifdef REALTIME_NOISE
   uint8 xor_tap;
#else
   boolean short_sample;
   int cur_pos;
#endif /* REALTIME_NOISE */

   /* for sync read $4105 */
   boolean enabled_cur;
   boolean holdnote_cur;
   long vbl_length_cur;
} noise_t;

typedef struct dmc_s
{
   uint8 regs[4];

   /* bodge for timestamp queue */
   boolean enabled;
   
   int32 freq;
   int32 phaseacc;
   int32 output_vol;

   //gli
   uint32 prev_addr;
   int prev_dmalength;

   uint32 address;
   uint32 cached_addr;
   int dma_length;
   int cached_dmalength;
   uint8 cur_byte;

   boolean looping;
   boolean irq_gen;
   boolean irq_occurred;

   /* for sync read $4105 and DPCM IRQ */
   int32 freq_cur;
   int32 phaseacc_cur;
   int dma_length_cur;
   int cached_dmalength_cur;
   boolean enabled_cur;
   boolean looping_cur;
   boolean irq_gen_cur;
   boolean irq_occurred_cur;
} dmc_t;

// ----------------------------------------------------------------------------
// APU Sound struct

enum
{
   APU_FILTER_NONE,
   APU_FILTER_LOWPASS,
   APU_FILTER_WEIGHTED
};

typedef struct
{
   uint32 min_range, max_range;
   uint8 (*read_func)(uint32 address);
} apu_memread;

typedef struct
{
   uint32 min_range, max_range;
   void (*write_func)(uint32 address, uint8 value);
} apu_memwrite;

/* external sound chip stuff */
typedef struct apuext_s
{
   void  (*init)(void);
   void  (*shutdown)(void);
   void  (*reset)(void);
   int32 (*process)(void);
   apu_memread *mem_read;
   apu_memwrite *mem_write;
} apuext_t;


typedef struct apu_s
{
   rectangle_t rectangle[2];
   triangle_t triangle;
   noise_t noise;
   dmc_t dmc;

   uint8 enable_reg;
   uint8 enable_reg_cur;

   uint32 elapsed_cycles;

   void *buffer; /* pointer to output buffer */
   int num_samples;

   boolean mix_enable[6];
   int filter_type;

   int32 cycle_rate;

   long sample_rate;
   int sample_bits;
   int refresh_rate;

   void (*process)(void *buffer, int num_samples);
} apu_t;

void Init_Sound();


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Function prototypes */
extern void apu_create(long sample_rate, long refresh_rate, long frag_size,
                         long sample_bits);
extern void apu_destroy(apu_t **apu);
extern void apu_setparams(long sample_rate, long refresh_rate, long frag_size,
                          long sample_bits);

extern void apu_process(void *buffer, int num_samples);
extern void apu_reset(void);

extern void apu_setfilter(long filter_type);
extern void apu_setchan(long chan, boolean enabled);

extern uint8 apu_read(uint32 address);

extern void apu_regwrite(uint32 address, uint8 value);
extern void apu_write_cur(uint32 address, uint8 value);

extern void sync_apu_register(void);
extern boolean sync_dmc_register(uint32 cpu_cycles);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NES_APU_H_ */

