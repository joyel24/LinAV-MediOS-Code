/************************************************************************

		Copyright (c) 2003 Brad Martin.

This file is part of OpenSPC.

OpenSPC is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OpenSPC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenSPC; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



dsp.c: implements functions that emulate the DSP part of the SPC chip.
Some of these functions cannot be static because the SPC core needs access
to them, however they are not intended for external library use, and their
specific implementations and prototypes are subject to change.

************************************************************************/

#undef DEBUG
#undef DBG_KEY
#undef DBG_ENV
#undef DBG_PMOD
#undef DBG_BRR
#undef DBG_ECHO
#undef DBG_INTRP

#undef NO_PMOD
#undef NO_ECHO
// #define NO_ECHO
/* #define NO_PMOD */


#define NENV 8


// #include <math.h>
// #include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "dsp.h"
#include "gauss.h"
}

// signed multiplication 31 bits * 16 bits --> 32 bits
// without call to $$MPY
inline long M_31_16(long a, short b)
{
  short ahi = a>>15;
  short alo = a&0x7fff;
  return ((long)ahi*b <<15) + (long)alo*b;
}

// 8 bits sign extension
#define ES(a) ( (a) - ( ((a)&0x80)<<1 ) )

/**** Global Variables :P ****/
int keyed_on,keys;	/* 8-bits for 8 voices */
struct voice_state voice_state[8];

/* These are for the FIR echo filter */
#ifndef NO_ECHO
static short FIRlbuf[8],FIRrbuf[8];
static int FIRptr,echo_ptr;
#endif

/* Noise stuff */
int noise_cnt,noise_lev;

static const int *G1=&gauss[256],
  *G2=&gauss[512],
  *G3=&gauss[255],
  *G4=&gauss[-1];	/* Ptrs to Gaussian table */

/* Original SPC DSP took samples 32000 times a second, which is once every
   (2048000/32000 = 64) cycles. */
const int TS_CYC=1024000/32000;

/* This table is for envelope timing.  It represents the number of counts
   that should be subtracted from the counter each sample period (32kHz). 
   The counter starts at 30720 (0x7800). */
static const int CNT_INIT=0x7800, ENVCNT[0x20]={
  0x0000,0x000F,0x0014,0x0018,0x001E,0x0028,0x0030,0x003C,
  0x0050,0x0060,0x0078,0x00A0,0x00C0,0x00F0,0x0140,0x0180,
  0x01E0,0x0280,0x0300,0x03C0,0x0500,0x0600,0x0780,0x0A00,
  0x0C00,0x0F00,0x1400,0x1800,0x1E00,0x2800,0x3C00,0x7800};

#include "port.h"
#include "apu.h"
#include "mmu.h"
#include "soundux.h"

#define DSPregs (APU.DSP)
#define READ_WORD(s) ( GET(s) | ( GET((s)+1)<<8 ) )

/**** Some macros ****/

/* make reading the ADSR code easier */

#define SL(v) (DSPregs[((v)<<4)+6]>>5)		/* Returns SUSTAIN level */
#define SR(v) (DSPregs[((v)<<4)+6]&0x1F)	/* Returns SUSTAIN rate */

/* handle endianness */
#ifdef WORDS_BIGENDIAN
#define LEtoME16(x) ((((x) >> 8) & 0xFF) | (((x) << 8) & 0xFF00))
#define MEtoLE16(x) ((((x) >> 8) & 0xFF) | (((x) << 8) & 0xFF00))
#else
#define LEtoME16(x) (x)
#define MEtoLE16(x) (x)
#endif

/**** Static functions ****/

static int AdvanceEnvelope(int v)	/* Return value is current ENVX */
{
  int envx=voice_state[v].envx,
    adsr1,t;
  long cnt;
  if(voice_state[v].envstate==RELEASE)
    {
      /* Docs: "When in the state of "key off". the "click" sound
	 is prevented by the addition of the fixed value 1/256"
	 WTF???  Alright, I'm going to choose to interpret that
	 this way:  When a note is keyed off, start the RELEASE
	 state, which subtracts 1/256th each sample period
	 (32kHz).  Note there's no need for a count because it
	 always happens every update. */
      envx-=0x8*NENV;  /* 0x8 / 0x800 = 1/256th */
      if(envx<=0)
	{
	  envx=0;
	  keys&=~(1<<v); 
	  return -1; 
	}
      voice_state[v].envx=envx;
      DSPregs[(v<<4)+8]=envx>>8;
#ifdef DBG_ENV
      fprintf(stderr,"ENV voice %d: envx=%03X, state=RELEASE\n",v,envx);
#endif
      return envx;
    }
  cnt=voice_state[v].envcnt;
  adsr1=DSPregs[(v<<4)+5];
  if(adsr1&0x80) switch(voice_state[v].envstate)
    {
    case ATTACK:
      /* Docs are very confusing.  "AR is multiplied by the fixed
	 value 1/64..."  I believe it means to add 1/64th to ENVX
	 once every time ATTACK is updated, and that's what I'm
	 going to implement. */
      t=adsr1&0xF;
      if(t==0xF)
	{
#ifdef DBG_ENV
	  fprintf(stderr,"ENV voice %d: instant attack\n",v);
#endif
	  envx+=NENV*0x400;
	}
      else
	{
	  cnt-=NENV*(long)ENVCNT[(t<<1)+1];
	  if(cnt>0)
	    break;
	  envx+=0x20; /* 0x020 / 0x800 = 1/64 */
	  cnt=CNT_INIT;
	}
      if(envx>0x7FF)
	{
	  envx=0x7FF;
	  voice_state[v].envstate=DECAY;
	}
#ifdef DBG_ENV
      fprintf(stderr,"ENV voice %d: envx=%03X, state=ATTACK\n",v,envx);
#endif
      voice_state[v].envx=envx;
      break;
    case DECAY:
      /* Docs: "DR... [is multiplied] by the fixed value 1-1/256."
	 Well, at least that makes some sense.  Multiplying ENVX
	 by 255/256 every time DECAY is updated. */
      cnt-=NENV*(long)ENVCNT[((adsr1>>3)&0xE)+0x10];
      if(cnt<=0)
	{
	  cnt=CNT_INIT;
	  envx-=((envx-1)>>8)+1;
	  voice_state[v].envx=envx;
	}
      if(envx<=0x100*(SL(v)+1))
	voice_state[v].envstate=SUSTAIN;
#ifdef DBG_ENV
      fprintf(stderr,"ENV voice %d: envx=%03X, state=DECAY\n",v,envx);
#endif
      break;
    case SUSTAIN:
      /* Docs: "SR [is multiplied] by the fixed value 1-1/256."
	 Multiplying ENVX by 255/256 every time SUSTAIN is
	 updated. */
#ifdef DBG_ENV
      if(ENVCNT[SR(v)]==0)
	fprintf(stderr,"ENV voice %d: envx=%03X, state=SUSTAIN, zero rate\n",v,envx);
#endif
      cnt-=NENV*(long)ENVCNT[SR(v)];
      if(cnt>0)
	break;
      cnt=CNT_INIT;
      envx-=((envx-1)>>8)+1;
#ifdef DBG_ENV
      fprintf(stderr,"ENV voice %d: envx=%03X, state=SUSTAIN\n",v,envx);
#endif
      voice_state[v].envx=envx;
      /* Note: no way out of this state except by explicit KEY OFF
	 (or switch to GAIN). */
      break;
    case RELEASE:	/* Handled earlier to prevent GAIN mode from stopping
			   KEY OFF events */
      break;
    }
  else
    {	/* GAIN mode is set */
      /* Note: if the game switches between ADSR and GAIN modes
	 partway through, should the count be reset, or should it
	 continue from where it was?  Does the DSP actually watch
	 for that bit to change, or does it just go along with
	 whatever it sees when it performs the update?  I'm going
	 to assume the latter and not update the count, unless I
	 see a game that obviously wants the other behavior.  The
	 effect would be pretty subtle, in any case. */
      t=DSPregs[(v<<4)+7];
      if(t<0x80)
	{
	  envx=voice_state[v].envx=t<<4;
#ifdef DBG_ENV
	  fprintf(stderr,"ENV voice %d: envx=%03X, state=DIRECT\n",v,envx);
#endif
	}
      else switch(t>>5)
	{
	case 4:	/* Docs: "Decrease (linear): Subtraction of the
		   fixed value 1/64." */
	  cnt-=NENV*(long)ENVCNT[t&0x1F];
	  if(cnt>0)
	    break;
	  cnt=CNT_INIT;
	  envx-=0x020;	/* 0x020 / 0x800 = 1/64th */
	  if(envx<0)
	    envx=0;
#ifdef DBG_ENV
	  fprintf(stderr,"ENV voice %d: envx=%03X, state=DECREASE\n",v,envx);
#endif
	  voice_state[v].envx=envx;
	  break;
	case 5: /* Docs: "Drecrease <sic> (exponential):
		   Multiplication by the fixed value 1-1/256." */
	  cnt-=NENV*(long)ENVCNT[t&0x1F];
	  if(cnt>0)
	    break;
	  cnt=CNT_INIT;
	  envx-=((envx-1)>>8)+1;
#ifdef DBG_ENV
	  fprintf(stderr,"ENV voice %d: envx=%03X, state=EXP\n",v,envx);
#endif
	  voice_state[v].envx=envx;
	  break;
	case 6: /* Docs: "Increase (linear): Addition of the fixed
		   value 1/64." */
	  cnt-=NENV*(long)ENVCNT[t&0x1F];
	  if(cnt>0)
	    break;
	  cnt=CNT_INIT;
	  envx+=0x020;	/* 0x020 / 0x800 = 1/64th */
	  if(envx>0x7FF)
	    envx=0x7FF;
#ifdef DBG_ENV
	  fprintf(stderr,"ENV voice %d: envx=%03X, state=INCREASE\n",v,envx);
#endif
	  voice_state[v].envx=envx;
	  break;
	case 7: /* Docs: "Increase (bent line): Addition of the
		   constant 1/64 up to .75 of the constaint <sic>
		   1/256 from .75 to 1." */
	  cnt-=NENV*(long)ENVCNT[t&0x1F];
	  if(cnt>0)
	    break;
	  cnt=CNT_INIT;
	  if(envx<0x600)		/* 0x600 / 0x800 = .75 */
	    envx+=0x020;	/* 0x020 / 0x800 = 1/64 */
	  else
	    envx+=0x008;	/* 0x008 / 0x800 = 1/256 */
	  if(envx>0x7FF)
	    envx=0x7FF;
#ifdef DBG_ENV
	  fprintf(stderr,"ENV voice %d: envx=%03X, state=INCREASE\n",v,envx);
#endif
	  voice_state[v].envx=envx;
	  break;
	}

    }
  voice_state[v].envcnt=cnt;
  DSPregs[(v<<4)+8]=envx>>4;
  return envx;
}


static void decodeblock(struct voice_state * restrict vp, int smp)
{
  short range, filter, * ptr, block[9], vl, vr;
  short * raw = vp->sampbuf;
  long outx, outx2;
  extern short FilterValues[4][2];
  static int shifts[16] = { 
    1,2,4,8,16,32,64,128,
    256*1,256*2,256*4,256*8,256*16,256*32,256*64,-256*128, };
  int i, f0, f1;

  *raw++ = vp->sampbuf[16];
  *raw++ = vp->sampbuf[17];
  *raw++ = vp->sampbuf[18];
  
  vp->rptr = smp;
  ptr = block;
  mmu_getbytes(smp, 9, block);

  vl=*ptr++;
  range=vl>>4;
  vp->end=vl&3;
  filter=(vl&12)>>2;
    
  for (i=0; i<8; i++) {
    vr=*ptr++;
    vl = (vr>>4) - ( (vr&0x80) >>(4-1) ); // sign extended
    vr = (vr&0xf) - ( (vr&0x8) <<1 ); // sign extended
    
    if(range<=0xC) {
      outx=((long)vl*shifts[range])>>1;
      outx2=((long)vr*shifts[range])>>1;
    } else {
      outx=vl&~0x7FF;
      outx2=vr&~0x7FF;
    }
    f0 = FilterValues[filter][0];
    f1 = FilterValues[filter][1];
    outx += (raw[-1] * (long)f0 + (long)raw[-2] * (long)f1) >>9;
    
    if(outx<-0x8000L)
      outx=-0x8000L;
    else if(outx>0x7FFFL)
      outx=0x7FFFL;
    *raw++=(outx<<1);

    f0 = FilterValues[filter][0];
    f1 = FilterValues[filter][1];
    outx2 += (raw[-1] * (long)f0 + (long)raw[-2] * (long)f1) >>9;
    
    if(outx2<-0x8000L)
      outx2=-0x8000L;
    else if(outx2>0x7FFFL)
      outx2=0x7FFFL;
    *raw++=(outx2<<1);
  }

  vp->sampptr = 0;
}



/**** Shared functions (for internal library use only) ****/

static  short sd;

static  short evl;
static  short evr;
static  short efb;

static  short mvl;
static  short mvr;

static  short vvl[8], vvr[8];
static  short pitch[8];

extern long MixBuffer [SOUND_BUFFER_SIZE];
extern long EchoBuffer [SOUND_BUFFER_SIZE];

void DSP_Update(short * restrict sound_ptr, int count)
{
  static short ecnt;
  int v,V,m,envx,vl,vr, J;
  short outl,outr,echol,echor;
#ifndef NO_ECHO
  int echo_base;
#endif
  signed long outx;	/* explicit type because it matters */
  struct voice_state * restrict vp;
  static short Loop[2][128+8];
  short * restrict loopl = Loop[0] + count - 1;
  short * restrict loopr = Loop[1] + count - 1;

  sd = (DSPregs[0x5D]<<8);
  
  evl = ES(DSPregs[0x2C]);
  evr = ES(DSPregs[0x3C]);
  efb = ES(DSPregs[0x0d]);
  mvl = ES(DSPregs[0x0c]);
  mvr = ES(DSPregs[0x1c]);

  for(v=0,m=1,V=0;v<8;v++,V+=16,m<<=1)
    {
      vp=&voice_state[v];
      vvl[v] = ES(DSPregs[v<<4]);
      vvr[v] = ES(DSPregs[(v<<4)+1]);

      if(DSPregs[0x4C]&m&~DSPregs[0x5C])
	{	/* Voice doesn't come on if key off is set */
	  DSPregs[0x4C]&=~m;
	  vp->on_cnt=8;
	}
      if(keys&DSPregs[0x5C]&m)
	{	/* voice was keyed off */
	  vp->envstate=RELEASE;
	  vp->on_cnt=0;
	}

      vp->fpitch=vp->pitch=(DSPregs[V+2] | (DSPregs[V+3]<<8) ) & 0x3FFF;
    }
	
  /* Check for reset */
  if(DSPregs[0x6C]&0x80)
    DSP_Reset();


  /* Here we check for keys on/off.  Docs say that successive writes
     to KON/KOF must be separated by at least 2 Ts periods or risk
     being neglected.  Therefore DSP only looks at these during an
     update, and not at the time of the write.  Only need to do this
     once however, since the regs haven't changed over the whole
     period we need to catch up with. */
  DSPregs[0x7C]&=~DSPregs[0x4C];	/* Keying on a voice resets that bit
					   in ENDX. */

  memset (MixBuffer, 0, count * sizeof (MixBuffer [0]));
  memset (EchoBuffer, 0, count * sizeof (EchoBuffer [0]));
  memset (MixBuffer+SOUND_BUFFER_SIZE/2, 0, count * sizeof (MixBuffer [0]));
  memset (EchoBuffer+SOUND_BUFFER_SIZE/2, 0, count * sizeof (EchoBuffer [0]));

  for (J=0; J<count; J++) {

  /* Question: what is the expected behavior when pitch
     modulation is enabled on voice 0?  Jurassic Park 2 does
     this.  For now, using outx of zero for first voice. */
  outx=0;
  if(DSPregs[0x3D])
    {	/* Same table for noise and envelope, yay! */
      noise_cnt-=ENVCNT[DSPregs[0x6C]&0x1F];
      if(noise_cnt<=0)
	{
	  static long z = 0x45826444;
	  static long r = 0;
	  if ((z <<= 1) & 0x80000000)
	    z ^= 0x40001;

	  noise_cnt=CNT_INIT;
	  noise_lev=z;
	}
    }
  outl=outr=echol=echor=0;
  for(v=0,m=1,V=0;v<8;v++,V+=16,m<<=1)
    {
      vp=&voice_state[v];
      if(vp->on_cnt&&(--vp->on_cnt==0))
	{	/* voice was keyed on */
	  keys|=m;
	  keyed_on|=m;
	  vl=DSPregs[(v<<4)+4];
	  //vp->samp_id=*(unsigned long *)&sd[vl];
	  decodeblock(vp, READ_WORD(sd + vl*4));
	  //vp->mem_ptr=sd[vl].vptr;
	  vp->envx=0;
	  vp->mixfrac=3*4096;
	  /* NOTE: Real SNES does *not* appear to initialize
	     the envelope counter to anything in particular.
	     The first cycle always seems to come at a random
	     time sooner than expected; as yet, I have been
	     unable to find any pattern.  I doubt it will
	     matter though, so we'll go ahead and do the full
	     time for now. */
	  vp->envcnt=CNT_INIT;
	  vp->envstate=ATTACK;
	}
      
      //if(!(keys&m)||((envx=AdvanceEnvelope(v))<0))

      if (!ecnt) AdvanceEnvelope(v);

      if(!(keys&m)||((envx=vp->envx)<0))
	{
	  DSPregs[V+8]=DSPregs[V+9]=outx=0;
	  continue;
	}
/*      vp->pitch=LEtoME16(
			 *((unsigned short *)&DSPregs[V+2]))&0x3FFF;*/
#ifndef NO_PMOD
      /* Pitch mod uses OUTX from last voice for this one.
	 Luckily we haven't modified OUTX since it was used
	 for last voice. */
      if(DSPregs[0x2D]&m)
	{
	  vp->pitch=M_31_16(outx+32768, vp->fpitch)>>15;
	}
#endif
      for(;vp->mixfrac>=0;vp->mixfrac-=4096)
	{
	  /* This part performs the BRR decode
	     'on-the-fly'.  This is more correct than
	     the old way, which could be fooled if the
	     data and/or the loop point changed while
	     the sample was playing, or if the BRR
	     decode didn't produce the same result every
	     loop because of the filters.  The event
	     interface still has no chance of keeping
	     up with those kinds of tricks, though. */
	  vp->sampptr++;
	  if(vp->sampptr == 16)
	    {
	      if(vp->end&1)
		{
		  /* Docs say ENDX bit is set
		     when decode of block with
		     source end flag set is
		     done.  Does this apply to
		     looping samples?  Some
		     info I've seen suggests
		     yes. */

		  DSPregs[0x7C]|=m;
		  if(vp->end&2)
		    {
		      decodeblock(vp, READ_WORD(sd + DSPregs[V+4] * 4 + 2));
		      //LEtoME16(sd[DSPregs[V+4]].lptr);
		    }
		  else
		    {
		      keys&=~m;
		      DSPregs[V+8]=
			vp->envx=0;
		      vp->sampptr = 0;
		      while(vp->mixfrac>=0)
			{
			  vp->sampbuf[vp->sampptr]=
			    outx=0;
			  vp->sampptr++;
			  vp->mixfrac-=4096;
			}
		      break;
		    }
		}
	      else
		{
		  decodeblock(vp, vp->rptr + 9);
		}
	    }

      
	}

      if(DSPregs[0x3D]&m)
	{
	  outx=noise_lev;
	}
      else
	{	/* Perform 4-Point Gaussian interpolation.
		   Take an approximation of a Gaussian
		   bell-curve, and move it through the
		   sample data at a rate determined by the
		   pitch.  The sample output at any given
		   time is the sum of the products of each
		   input sample point with the value of the
		   bell-curve corresponding to that point. */
	  vl=vp->mixfrac>>4;
	  outx=((long)G4[-vl]*vp->sampbuf[vp->sampptr])/*&~2047*/;
	  outx+=((long)G3[-vl]*vp->sampbuf[(vp->sampptr+1)])/*&~2047*/;
	  outx+=((long)G2[vl]*vp->sampbuf[(vp->sampptr+2)])/*&~2047*/;
	  outx+=((long)G1[vl]*vp->sampbuf[(vp->sampptr+3)])/*&~2047*/;
	  outx=outx>>11/*&~1*/;
	}
		
      /* Advance the sample position for next update. */
      vp->mixfrac+=vp->pitch;
		
      outx=(((long)outx*envx)>>11)/*&~1*/;
      DSPregs[V+9]=outx>>8;

      vl=M_31_16(outx, vvl[v])>>7;
      vr=M_31_16(outx, vvr[v])>>7;
      MixBuffer[J] += vl;
      MixBuffer[SOUND_BUFFER_SIZE/2 + J] += vr;
      if(DSPregs[0x4D]&m)
	{
	  EchoBuffer[J] += vl;
	  EchoBuffer[SOUND_BUFFER_SIZE/2 + J] += vr;
	}
    }

    ecnt = (ecnt+1)&(NENV-1);
  }

  for (J=0; J<count; J++) {

    outl=M_31_16((long)MixBuffer[J],mvl)>>7;
    outr=M_31_16((long)MixBuffer[SOUND_BUFFER_SIZE/2+J],mvr)>>7;
#ifndef NO_ECHO
  /* Perform echo.  First, read mem at current location, and
     put those samples into the FIR filter queue. */
  echo_base=((DSPregs[0x6D]<<8)+echo_ptr)&0xFFFF;
	
  {
    unsigned short buf[4];
    long vl, vr;
    extern int FilterTaps [8];

  /* 	FIRlbuf[FIRptr]=LEtoME16( */
  /* 	 *(signed short *)&SPC_RAM[echo_base]); */
  /* 	FIRrbuf[FIRptr]=LEtoME16( */
  /* 	 *(signed short *)&SPC_RAM[echo_base+sizeof(short)]); */

  /* Now, evaluate the FIR filter, and add the results
     into the final output. */
    int * restrict filt = FilterTaps;
    short e, * restrict loop;

    mmu_getbytes(echo_base, 4, (short *) buf);

    e = buf[0]+(buf[1]<<8);
    loop = loopl;
    *loop++ = e;
    vl =  (long) e        * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    vl += (long) *loop++ * (long) *filt++;
    loopl--;
    outl+=M_31_16((long)vl, evl)>>14;

    filt = FilterTaps;
    e = buf[2]+(buf[3]<<8);
    loop = loopr;
    *loop++ = e;
    vr =  (long) e        * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    vr += (long) *loop++ * (long) *filt++;
    loopr--;
    outr+=M_31_16((long)vr, evr)>>14;

  if(!(DSPregs[0x6C]&0x20))
    {	/* Add the echo feedback back into the original
	   result, and save that into memory for use later. */
      echol = EchoBuffer[J] + (M_31_16(vl, efb)>>14);
      if(echol>32767)
	echol=32767;
      else if(echol<-32768)
	echol=-32768;
      echor = EchoBuffer[SOUND_BUFFER_SIZE/2+J] + (M_31_16(vr, efb)>>14);
      if(echor>32767)
	echor=32767;
      else if(echor<-32768)
	echor=-32768;
      {
	unsigned short buf[4];
	buf[0] = echol&0xff;
	buf[1] = echol>>8;
	buf[2] = echor&0xff;
	buf[3] = echor>>8;
	mmu_setbytes(echo_base, 4, (short *) buf);
/* 	mmu_setbyte(echo_base, buf[0]); */
/* 	mmu_setbyte(echo_base+1, buf[1]); */
/* 	mmu_setbyte(echo_base+2, buf[2]); */
/* 	mmu_setbyte(echo_base+3, buf[3]); */


/* 		*(signed short *)&SPC_RAM[echo_base]=MEtoLE16(echol); */
/* 		*(signed short *)&SPC_RAM[echo_base+sizeof(short)]= */
/* 		  MEtoLE16(echor); */
      }
    }
  }
  echo_ptr+=2*2; //sizeof(short);
  if(echo_ptr>=((DSPregs[0x7D]&0xF)<<11))
    echo_ptr=0;
#endif
  //if(sound_ptr)
    {
      if(0 && DSPregs[0x6C]&0x40)
	{	/* MUTE */
	  *sound_ptr=0;
	  sound_ptr++;
	  *sound_ptr=0;
	  sound_ptr++;
	}
      else
	{
	  if(outl>32767L)
	    *sound_ptr=32767L;
	  else if(outl<-32768L)
	    *sound_ptr=-32768L;
	  else
	    *sound_ptr=outl;
	  sound_ptr++;
	  if(outr>32767L)
	    *sound_ptr=32767L;
	  else if(outr<-32768L)
	    *sound_ptr=-32768L;
	  else
	    *sound_ptr=outr;
	  sound_ptr++;
	}
    }
  /*	DSPregs[0x4C]=0;*/

  }

  memcpy(Loop[0] + count, Loop[0], 8*sizeof(Loop[0][0]));
  memcpy(Loop[1] + count, Loop[1], 8*sizeof(Loop[0][0]));
}

void DSP_Reset(void)
{
  int i;
#ifdef DEBUG
  fprintf(stderr,"DSP_Reset\n");
#endif
  for(i=0;i<8;i++)
#ifndef NO_ECHO
    FIRlbuf[i]=FIRrbuf[i]=
#endif
      voice_state[i].on_cnt=0;
#ifndef NO_ECHO
  FIRptr=echo_ptr=0;
#endif
  keys=keyed_on=noise_cnt=0;
  DSPregs[0x6C]|=0xE0;
  DSPregs[0x4C]=DSPregs[0x5C]=0;
}
