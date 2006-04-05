#define HAVE_ECHO

/*
 * Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
 *
 * (c) Copyright 1996 - 2000 Gary Henderson (gary@daniver.demon.co.uk) and
 *                           Jerremy Koot (jkoot@snes9x.com)
 *
 * Super FX C emulator code 
 * (c) Copyright 1997 - 1999 Ivar (Ivar@snes9x.com) and
 *                           Gary Henderson.
 * Super FX assembler emulator code (c) Copyright 1998 zsKnight and _Demo_.
 *
 * DSP1 emulator code (c) Copyright 1998 Ivar, _Demo_ and Gary Henderson.
 * DOS port code contains the works of other authors. See headers in
 * individual files.
 *
 * Snes9x homepage: www.snes9x.com
 *
 * Permission to use, copy, modify and distribute Snes9x in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Snes9x is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Snes9x or software derived from Snes9x.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so everyone can benefit from the modifications
 * in future versions.
 *
 * Super NES and Super Nintendo Entertainment System are trademarks of
 * Nintendo Co., Limited and its subsidiary companies.
 */

#ifdef __DJGPP__
#include <allegro.h>
#undef TRUE
#endif

#include <stdlib.h>
// #include <stdio.h>
#include <string.h>

#include <intrindefs.h>

#include "gauss.h"

static const int *G1=&gauss[256],
                 *G2=&gauss[512],
                 *G3=&gauss[255],
                 *G4=&gauss[-1];	/* Ptrs to Gaussian table */


// signed multiplication 31 bits * 16 bits --> 32 bits
// without call to $$MPY
inline long M_31_16(long a, short b)
{
  short ahi = a>>15;
  short alo = a&0x7fff;
  return ((long)ahi*b <<15) + (long)alo*b;
}


#if 0
// VP tried with no jump, but less efficient at the end of the day
#undef CLIP16
inline void CLIP16(long & x)
{
  const short a = -32678, b = 32767;
  x -= a;
  x &= (~x) >> 31;
  x += a;
  x -= b;
  x &= x >> 31;
  x += b;
}
#define CLIP16_latch(v, l) CLIP16(v)
#else
#define CLIP16(v) \
if ((v) < -32768) \
    (v) = -32768; \
else \
if ((v) > 32767) \
    (v) = 32767

// VP disabled latch noise detection
#define CLIP16_latch(v,l) \
if ((v) < -32768) \
{ (v) = -32768; /*(l)++;*/ }\
else \
if ((v) > 32767) \
{ (v) = 32767; /*(l)++;*/ }
#endif

#define CLIP24(v) \
if ((v) < -8388608) \
    (v) = -8388608; \
else \
if ((v) > 8388607) \
    (v) = 8388607

#define CLIP8(v) \
if ((v) < -128) \
    (v) = -128; \
else \
if ((v) > 127) \
    (v) = 127

// 8 bits sign extension
#define ES(a) ( (a) - ( ((a)&0x80)<<1 ) )

#include "snes9x.h"
#include "soundux.h"
#include "apu.h"
#include "memmap.h"
// #include "cpuexec.h"
#include "mmu.h"

#include "../dspshared.h"

extern "C" {
// direction : 1 dsp-->sdram, 0 sdram-->dsp
int dma_dsp2sdram(void * dsp_addr, unsigned long sdram_addr, short length, short direction);
int dma_pending();
extern int dma_inuse;
}

extern "C" void prof(int id);

//extern int Echo [24000];
extern long DummyEchoBuffer [SOUND_BUFFER_SIZE];
extern long MixBuffer [SOUND_BUFFER_SIZE];
extern long EchoBuffer [SOUND_BUFFER_SIZE];
extern int FilterTaps [8];
extern short Z;
//extern short Loop [16];

extern int FilterValues[4][2];
extern int NoiseFreq [32];

#undef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))

#define FIXED_POINT 0x10000UL
#define FIXED_POINT_REMAINDER 0xffffUL
#define FIXED_POINT_SHIFT 16

#define VOL_DIV8  0x8000
#define VOL_DIV16 0x0080
#define ENVX_SHIFT 24

// F is channel's current frequency and M is the 16-bit modulation waveform
// from the previous channel multiplied by the current envelope volume level.
//#define PITCH_MOD(F,M) ((F) * ((((unsigned long) (M)) + (0x800000>>16)) /*>> 16*/) >> 7)
#define PITCH_MOD(F,M) ((F) * ((((long) (M)))) >> 14)
//#define PITCH_MOD(F,M) ((long)(F) * ((((long)(M) & (0x7fffff>>12)) /*>> 16*/) + 1) >> 8)
//#define PITCH_MOD(F,M) ((F) * ((((M) & 0x7fffff) >> 14) + 1) >> 8)

#define LAST_SAMPLE 0xffffff
#define JUST_PLAYED_LAST_SAMPLE(c) ((c)->sample_pointer >= LAST_SAMPLE)


#define BCOLOR(r,g,b)


#define READ_WORD(s) ( GET(s) | ( GET((s)+1)<<8 ) )
//#define READ_WORD(s) ( GET(s+1) | ( GET((s))<<8 ) )


STATIC inline unsigned short S9xGetSampleAddress (int sample_number)
{
    return (((APU.DSP[APU_DIR] << 8) + (sample_number << 2))/* & 0xffff*/);
}

void S9xAPUSetEndOfSample (int i, Channel *ch)
{
    ch->state = SOUND_SILENT;
    ch->mode = MODE_NONE;
    APU.DSP [APU_ENDX] |= 1 << i;
    APU.DSP [APU_KON] &= ~(1 << i);
    APU.DSP [APU_KOFF] &= ~(1 << i);
    APU.KeyedChannels &= ~(1 << i);
}
#ifdef __DJGPP
END_OF_FUNCTION (S9xAPUSetEndOfSample)
#endif

void S9xAPUSetEndX (int ch)
{
    APU.DSP [APU_ENDX] |= 1 << ch;
}
#ifdef __DJGPP
END_OF_FUNCTION (S9xAPUSetEndX)
#endif

void S9xSetEnvRate (Channel *ch, unsigned long rate, int direction, int target)
{
    ch->envx_target = target;

    if (rate == ~0UL)
    {
	ch->direction = 0;
	rate = 0;
    }
    else
	ch->direction = direction;

    static int steps [] =
    {
//	0, 64, 1238, 1238, 256, 1, 64, 109, 64, 1238
	0, 64, 619, 619, 128, 1, 64, 55, 64, 619
    };

    if (rate == 0 || so.playback_rate == 0)
	ch->erate = 0;
    else
    {
	ch->erate = (unsigned long)
// 	  (((float) FIXED_POINT * 1000.0f * steps [ch->state]) /
// 	   (rate * (long)so.playback_rate));
	  ((((int32) 1000L * steps [ch->state]) << 8) /
	   ((rate * (int32)so.playback_rate) >> 8));
// 		    (((int32) FIXED_POINT * 1000L * steps [ch->state]) /
// 		     (rate * (long)so.playback_rate));
    }
}

#ifdef __DJGPP
END_OF_FUNCTION(S9xSetEnvRate);
#endif

void S9xSetEnvelopeRate (int channel, unsigned long rate, int direction,
			 int target)
{
    S9xSetEnvRate (&SoundData.channels [channel], rate, direction, target);
}

#ifdef __DJGPP
END_OF_FUNCTION(S9xSetEnvelopeRate);
#endif

void S9xSetSoundVolume (int channel, short volume_left, short volume_right)
{
  // VP bug test , put these back absolutely
  volume_left = ES(volume_left);
  volume_right = ES(volume_right);
/*     if (volume_left >= 128) volume_left = 127;
     if (volume_right >= 128) volume_right = 127; */

    Channel *ch = &SoundData.channels[channel];
    if (!so.stereo)
      volume_left = (ABS(volume_right) + ABS(volume_left)) >>1;

    ch->volume_left = volume_left;
    ch->volume_right = volume_right;
    ch-> left_vol_level = (ch->envx * volume_left)/* / 128*/;
    ch->right_vol_level = (ch->envx * volume_right)/* / 128*/;
}

void S9xSetMasterVolume (short volume_left, short volume_right)
{
    volume_left = ES(volume_left);
    volume_right = ES(volume_right);
    if (Settings.DisableMasterVolume)
    {
	SoundData.master_volume_left = 127;
	SoundData.master_volume_right = 127;
	SoundData.master_volume [0] = SoundData.master_volume [1] = 127;
    }
    else
    {
	if (!so.stereo)
	  volume_left = (ABS (volume_right) + ABS (volume_left)) >>1;
	SoundData.master_volume_left = volume_left;
	SoundData.master_volume_right = volume_right;
	SoundData.master_volume [0] = volume_left;
	SoundData.master_volume [1] = volume_right;
    }
}

void S9xSetEchoVolume (short volume_left, short volume_right)
{
    volume_left = ES(volume_left);
    volume_right = ES(volume_right);
    if (!so.stereo)
      volume_left = (ABS (volume_right) + ABS (volume_left)) >>1;
    SoundData.echo_volume_left = volume_left;
    SoundData.echo_volume_right = volume_right;
    SoundData.echo_volume [0] = volume_left;
    SoundData.echo_volume [1] = volume_right;
}

void S9xSetEchoEnable (uint8 byte)
{
    SoundData.echo_channel_enable = byte;
    if (!SoundData.echo_write_enabled || Settings.DisableSoundEcho)
	byte = 0;
    if (byte && !SoundData.echo_enable)
    {
#ifdef HAVE_ECHO
      //memset (Echo, 0, sizeof (Echo));
#endif
      //memset (Loop, 0, sizeof (Loop));
    }

    SoundData.echo_enable = byte;
    for (int i = 0; i < 8; i++)
    {
	if (byte & (1 << i))
	    SoundData.channels [i].echo_buf_ptr = EchoBuffer;
	else
	    SoundData.channels [i].echo_buf_ptr = DummyEchoBuffer;
    }
}

void S9xSetEchoFeedback (int feedback)
{
    feedback = ES(feedback);
    CLIP8(feedback);
    SoundData.echo_feedback = feedback;
}

void S9xSetEchoDelay (int delay)
{
    SoundData.echo_buffer_size = (512 * (long) delay * (long) so.playback_rate) / 32000;
    if (so.stereo)
	SoundData.echo_buffer_size <<= 1;
    if (SoundData.echo_buffer_size)
	SoundData.echo_ptr %= SoundData.echo_buffer_size;
    else
	SoundData.echo_ptr = 0;
    S9xSetEchoEnable (APU.DSP [APU_EON]);
}

void S9xSetEchoWriteEnable (uint8 byte)
{
    SoundData.echo_write_enabled = byte;
    S9xSetEchoDelay (APU.DSP [APU_EDL] & 15);
}

void S9xSetFrequencyModulationEnable (uint8 byte)
{
    SoundData.pitch_mod = byte & ~1;
}

void S9xSetSoundKeyOff (int channel)
{
    Channel *ch = &SoundData.channels[channel];

    if (ch->state != SOUND_SILENT)
    {
	ch->state = SOUND_RELEASE;
	ch->mode = MODE_RELEASE;
	S9xSetEnvRate (ch, 8, -1, 0);
    }
}

void S9xFixSoundAfterSnapshotLoad ()
{
    SoundData.echo_write_enabled = !(APU.DSP [APU_FLG] & 0x20);
    SoundData.echo_channel_enable = APU.DSP [APU_EON];
    S9xSetEchoDelay (APU.DSP [APU_EDL] & 0xf);
    S9xSetEchoFeedback ((signed char) APU.DSP [APU_EFB]);

    S9xSetFilterCoefficient (0, (signed char) APU.DSP [APU_C0]);
    S9xSetFilterCoefficient (1, (signed char) APU.DSP [APU_C1]);
    S9xSetFilterCoefficient (2, (signed char) APU.DSP [APU_C2]);
    S9xSetFilterCoefficient (3, (signed char) APU.DSP [APU_C3]);
    S9xSetFilterCoefficient (4, (signed char) APU.DSP [APU_C4]);
    S9xSetFilterCoefficient (5, (signed char) APU.DSP [APU_C5]);
    S9xSetFilterCoefficient (6, (signed char) APU.DSP [APU_C6]);
    S9xSetFilterCoefficient (7, (signed char) APU.DSP [APU_C7]);
    for (int i = 0; i < 8; i++)
    {
	SoundData.channels[i].needs_decode = TRUE;
	S9xSetSoundFrequency (i, SoundData.channels[i].hertz);
	SoundData.channels [i].envxx = (long)SoundData.channels [i].envx << ENVX_SHIFT;
	//SoundData.channels [i].interpolate = 0;
	SoundData.channels [i].latch_noise = 0;
    }
    SoundData.master_volume [0] = SoundData.master_volume_left;
    SoundData.master_volume [1] = SoundData.master_volume_right;
    SoundData.echo_volume [0] = SoundData.echo_volume_left;
    SoundData.echo_volume [1] = SoundData.echo_volume_right;
    IAPU.Scanline = 0;
}

void S9xSetFilterCoefficient (int tap, int value)
{
    FilterTaps [tap & 7] = ES(value);
    SoundData.no_filter = (FilterTaps [0] == 127 || FilterTaps [0] == 0) && 
			   FilterTaps [1] == 0   &&
			   FilterTaps [2] == 0   &&
			   FilterTaps [3] == 0   &&
			   FilterTaps [4] == 0   &&
			   FilterTaps [5] == 0   &&
			   FilterTaps [6] == 0   &&
			   FilterTaps [7] == 0;
}

void S9xSetSoundADSR (int channel, int attack_rate, int decay_rate,
		      int sustain_rate, int sustain_level, int release_rate)
{
    SoundData.channels[channel].attack_rate = attack_rate;
    SoundData.channels[channel].decay_rate = decay_rate;
    SoundData.channels[channel].sustain_rate = sustain_rate;
    SoundData.channels[channel].release_rate = release_rate;
    SoundData.channels[channel].sustain_level = sustain_level + 1;

    switch (SoundData.channels[channel].state)
    {
    case SOUND_ATTACK:
	S9xSetEnvelopeRate (channel, attack_rate, 1, 127);
	break;

    case SOUND_DECAY:
	S9xSetEnvelopeRate (channel, decay_rate, -1,
			    (MAX_ENVELOPE_HEIGHT * (sustain_level + 1)) >> 3);
	break;
    case SOUND_SUSTAIN:
	S9xSetEnvelopeRate (channel, sustain_rate, -1, 0);
	break;
    }
}

void S9xSetEnvelopeHeight (int channel, int level)
{
    Channel *ch = &SoundData.channels[channel];

    ch->envx = level;
    ch->envxx = (long)level << ENVX_SHIFT;

    ch->left_vol_level = (level * ch->volume_left)/* / 128*/;
    ch->right_vol_level = (level * ch->volume_right)/* / 128*/;

    if (ch->envx == 0 && ch->state != SOUND_SILENT && ch->state != SOUND_GAIN)
    {
	S9xAPUSetEndOfSample (channel, ch);
    }
}

int S9xGetEnvelopeHeight (int channel)
{
    if (Settings.SoundEnvelopeHeightReading &&
	SoundData.channels[channel].state != SOUND_SILENT &&
	SoundData.channels[channel].state != SOUND_GAIN)
    {
	return (SoundData.channels[channel].envx);
    }
    return (0);
}

#if 1
void S9xSetSoundSample (int, uint16) 
{
}
#else
void S9xSetSoundSample (int channel, uint16 sample_number)
{
    register Channel *ch = &SoundData.channels[channel];

    if (ch->state != SOUND_SILENT && 
	sample_number != ch->sample_number)
    {
	int keep = ch->state;
	ch->state = SOUND_SILENT;
	ch->sample_number = sample_number;
	ch->loop = FALSE;
	ch->needs_decode = TRUE;
	ch->last_block = FALSE;
	memset(ch->decoded+15, 0, 4);
	unsigned short dir = S9xGetSampleAddress (sample_number);
	ch->block_pointer = READ_WORD (dir);
	ch->sample_pointer = 0;
	ch->state = keep;
    }
}
#endif

void S9xSetSoundFrequency (int channel, uint32 hertz)
{
    if (so.playback_rate)
    {
	if (SoundData.channels[channel].type == SOUND_NOISE)
	    hertz = NoiseFreq [APU.DSP [APU_FLG] & 0x1f];
 	SoundData.channels[channel].frequency = hertz;
// 	  ((((float) hertz) * FIXED_POINT) / so.playback_rate);
 	  //((unsigned long)hertz <<14)/((long)so.playback_rate>>2);
	  //(((int32) (hertz)) <<8) / (((long)so.playback_rate)>>8); // VP check
    // assume FIXED_POINT to be 0x10000
    }
}

void S9xSetSoundHertz (int channel, uint32 hertz)
{
    SoundData.channels[channel].hertz = hertz;
    S9xSetSoundFrequency (channel, hertz);
}

void S9xSetSoundType (int channel, int type_of_sound)
{
    SoundData.channels[channel].type = type_of_sound;
}

bool8 S9xSetSoundMute (bool8 mute)
{
    bool8 old = so.mute_sound;
    so.mute_sound = mute;
    return (old);
}

void DecodeBlock (Channel * restrict ch)
{
    int32 out;
    unsigned short filter;
    unsigned short shift;
    short sample1, sample2;
    short i;

    dsp_com->spc_decodes++;

    if (ch->block_pointer > 0xffff - 8)
    {
	ch->last_block = TRUE;
	ch->loop = FALSE;
	return;
    }

    //prof(4);

    short buf[9];
    mmu_getbytes(ch->block_pointer, 9, buf);
    short * restrict compressed = buf;

    BCOLOR(0, 255, 255);

    filter = *compressed;
    if ((ch->last_block = filter & 1))
	ch->loop = (filter & 2) != 0;

    compressed++;
    signed short * restrict raw = ch->decoded;
    
    *raw++ = ch->decoded[16];
    *raw++ = ch->decoded[17];
    *raw++ = ch->decoded[18];
    
    shift = filter >> 4;
    filter = ((filter >> 2) & 3);
    int f0 = FilterValues[filter][0];
    int f1 = FilterValues[filter][1];
    int latch_noise = 0;
    
    for (i = 8; i != 0; i--)
      {
	short val = *compressed++;
	sample2 = (val&0xf) - ( (val&0x8) <<1 ); // sign extended
	sample1 = (val>>4) - ( (val&0x80) >>(4-1) ); // sign extended
	    
	//out = (((long)sample1) << shift);
	//out = ( (long) (sample1>>16-shift) << 16 ) | ( sample1 << shift );
	// VP taking advantage of the 1-cycle 16*16 -> 32 multiplication opcode
	static int shifts[16] = { 
	  1,2,4,8,16,32,64,128,
	  256*1,256*2,256*4,256*8,256*16,256*32,256*64,-256*128, };
	if(shift<=0xC)
	  out = (long)sample1 * ((long)shifts[shift]) >>1;
	else
	  out = sample1&0x7ff;
	out += ((long)raw[-1] * (long)f0 + (long)raw[-2] * (long)f1) >>9;
	
	CLIP16_latch(out,latch_noise);
	*raw++ = (signed short) out <<1;
	
	//out = (((long)sample2) << shift);
	//out = ( (long) (sample2>>16-shift) << 16 ) | ( sample2 << shift );
	if(shift<=0xC)
	  out = (long)sample2 *  ((long)shifts[shift]) >>1;
	else
	  out = sample2&0x7ff;
	out += ((long)raw[-1] * (long)f0 + (long)raw[-2] * (long)f1) >>9;
	
	CLIP16_latch(out,latch_noise);
	*raw++ = (signed short) out<<1;
      }
    
    ch->block_pointer += 9;
    if (ch->block_pointer >= 0xffff - 8)
      {
	ch->last_block = TRUE;
	ch->loop = FALSE;
	ch->block_pointer -= 0xffff - 8;
      }
    
    BCOLOR(255, 255, 0);
    
    //prof(6);
}

/* VP : Moved into a static array, it used to be allocated on the stack !! */
static int wave[SOUND_BUFFER_SIZE];

void MixStereo (int sample_count)
{

/*

cubic filter  samples   [ x , y , z , w ]

coeficients [a,b,c,d] :

[
 1/2 z - 1/6 w - 1/2 y + 1/6 x
 1/4 w - 1/4 z - 1/4 y + 1/4 x, 
 1/24 w - 9/8 z + 9/8 y - 1/24 x, 
 9/16 z - 1/16 w + 9/16 y - 1/16 x, 
]


interpolated[p] = a p^3 + b p^2 + c p + d
where
 -1/2 <= p <= 1/2

*/

    int pitch_mod = SoundData.pitch_mod & ~APU.DSP[APU_NON];
    int J;
    int m;

    for (J = 0, m = 1; J < NUM_CHANNELS; J++, m <<= 1) 
    {
	int VL, VR;
	Channel * restrict ch = &SoundData.channels[J];
#define FSHIFT (16-4)  // VP try with only 4 later
#define FFIXED (1<<FSHIFT)
	int freq0 = ch->frequency/*>>(16-FSHIFT)*/;

	int mod = pitch_mod & m;
	int mod2 = pitch_mod & (m<<1);

	if (ch->state == SOUND_SILENT/* && !mod2 && !mod*/ /* || !(so.sound_switch & (1 << J))*/)
	    continue;

	dsp_com->spc_channels++;

	//freq0 = (unsigned long) ((double) freq0 * 0.985);
	//freq0 /= 2;

	//freq0 = (long)freq0 * ((long)(0.985*0x10000)) >>16;
	//freq0 = (long)freq0 * ((long)(0.75*0x10000)) >>16;

	if (ch->needs_decode) 
	{
	    DecodeBlock(ch);
	    ch->needs_decode = FALSE;
	    ch->sample_pointer = 0; //freq0 >> FSHIFT;
// 	    if (ch->sample_pointer == 0)
// 		ch->sample_pointer = 1;
// 	    if (ch->sample_pointer > SOUND_DECODE_LENGTH)
// 		ch->sample_pointer = SOUND_DECODE_LENGTH - 1;
	}
// 	VL = (((long) ch->sample) * ((long) ch-> left_vol_level)) >>14;
// 	VR = (((long) ch->sample) * ((long) ch->right_vol_level)) >>14;

	int I;
	int freq = freq0;
	for (I = 0; I < sample_count; I += 2)
	{

	    if (mod) {
	      //freq = PITCH_MOD(freq0, wave [I>>1]);
	      freq = (long)freq0 * wave [I>>1] >> 14;
	    }

#if 1
	    ch->env_error += ch->erate;
	    if (ch->env_error >= FIXED_POINT) 
	    {
	        BCOLOR(0, 0, 255);
		uint32 step = ch->env_error >> FIXED_POINT_SHIFT;

		switch (ch->state)
		{
		case SOUND_ATTACK:
		    ch->env_error &= FIXED_POINT_REMAINDER;
		    ch->envx += step << 1;
		    ch->envxx = (long)ch->envx << ENVX_SHIFT;

		    if (ch->envx >= 126)
		    {
			ch->envx = 127;
			ch->envxx = 127L << ENVX_SHIFT;
			ch->state = SOUND_DECAY;
			if (ch->sustain_level != 8) 
			{
			    S9xSetEnvRate (ch, ch->decay_rate, -1,
						(MAX_ENVELOPE_HEIGHT * ch->sustain_level)
						>> 3);
			    break;
			}
			ch->state = SOUND_SUSTAIN;
			S9xSetEnvRate (ch, ch->sustain_rate, -1, 0);
		    }
		    break;
		
		case SOUND_DECAY:
		    while (ch->env_error >= FIXED_POINT)
		    {
			ch->envxx = (ch->envxx >> 8) * 255;
			ch->env_error -= FIXED_POINT;
		    }
		    ch->envx = ch->envxx >> ENVX_SHIFT;
		    if (ch->envx <= ch->envx_target)
		    {
			if (ch->envx <= 0)
			{
			    S9xAPUSetEndOfSample (J, ch);
			    goto stereo_exit;
			}
			ch->state = SOUND_SUSTAIN;
			S9xSetEnvRate (ch, ch->sustain_rate, -1, 0);
		    }
		    break;

		case SOUND_SUSTAIN:
		    while (ch->env_error >= FIXED_POINT)
		    {
			ch->envxx = (ch->envxx >> 8) * 255;
			ch->env_error -= FIXED_POINT;
		    }
		    ch->envx = ch->envxx >> ENVX_SHIFT;
		    if (ch->envx <= 0)
		    {
			S9xAPUSetEndOfSample (J, ch);
			goto stereo_exit;
		    }
		    break;
		    
		case SOUND_RELEASE:
		    while (ch->env_error >= FIXED_POINT)
		    {
		      ch->envxx -= ((long)MAX_ENVELOPE_HEIGHT << ENVX_SHIFT) >>8;
			ch->env_error -= FIXED_POINT;
		    }
		    ch->envx = ch->envxx >> ENVX_SHIFT;
		    if (ch->envx <= 0)
		    {
			S9xAPUSetEndOfSample (J, ch);
			goto stereo_exit;
		    }
		    break;
		
		case SOUND_INCREASE_LINEAR:
		    ch->env_error &= FIXED_POINT_REMAINDER;
		    ch->envx += step << 1;
		    ch->envxx = (long)ch->envx << ENVX_SHIFT;

		    if (ch->envx >= 126)
		    {
			ch->envx = 127;
			ch->envxx = 127L << ENVX_SHIFT;
			ch->state = SOUND_GAIN;
			ch->mode = MODE_GAIN;
			S9xSetEnvRate (ch, 0, -1, 0);
		    }
		    break;

		case SOUND_INCREASE_BENT_LINE:
		    if (ch->envx >= (MAX_ENVELOPE_HEIGHT * 3) / 4)
		    {
			while (ch->env_error >= FIXED_POINT)
			{
			  ch->envxx += ((long)MAX_ENVELOPE_HEIGHT << ENVX_SHIFT) >>8;
			    ch->env_error -= FIXED_POINT;
			}
			ch->envx = ch->envxx >> ENVX_SHIFT;
		    }
		    else
		    {
			ch->env_error &= FIXED_POINT_REMAINDER;
			ch->envx += step << 1;
			ch->envxx = (long)ch->envx << ENVX_SHIFT;
		    }

		    if (ch->envx >= 126)
		    {
			ch->envx = 127;
			ch->envxx = 127L << ENVX_SHIFT;
			ch->state = SOUND_GAIN;
			ch->mode = MODE_GAIN;
			S9xSetEnvRate (ch, 0, -1, 0);
		    }
		    break;

		case SOUND_DECREASE_LINEAR:
		    ch->env_error &= FIXED_POINT_REMAINDER;
		    ch->envx -= step << 1;
		    ch->envxx = (long)ch->envx << ENVX_SHIFT;
		    if (ch->envx <= 0)
		    {
			S9xAPUSetEndOfSample (J, ch);
			goto stereo_exit;
		    }
		    break;

		case SOUND_DECREASE_EXPONENTIAL:
		    while (ch->env_error >= FIXED_POINT)
		    {
			ch->envxx = (ch->envxx >> 8) * 255;
			ch->env_error -= FIXED_POINT;
		    }
		    ch->envx = ch->envxx >> ENVX_SHIFT;
		    if (ch->envx <= 0)
		    {
			S9xAPUSetEndOfSample (J, ch);
			goto stereo_exit;
		    }
		    break;
		
		case SOUND_GAIN:
		    S9xSetEnvRate (ch, 0, -1, 0);
		    break;
		}
		ch-> left_vol_level = (ch->envx * ch->volume_left)/* >>14*/;
		ch->right_vol_level = (ch->envx * ch->volume_right)/* >>14*/;
// 		VL = ((long) ch->sample * (long)ch-> left_vol_level) >>14;
// 		VR = ((long) ch->sample * (long)ch->right_vol_level) >>14;

	        BCOLOR(255, 255, 0);
	    }
#else
	    ch->envx = 128;
	    ch-> left_vol_level = (ch->envx * ch->volume_left)/* >>14*/;
	    ch->right_vol_level = (128 * ch->volume_right)/* >>14*/;
// 	    VL = ((long) ch->sample * (long)ch-> left_vol_level) >>14;
// 	    VR = ((long) ch->sample * (long)ch->right_vol_level) >>14;
#endif

	    ch->count += freq;
	    if (ch->count >= FFIXED)
	    {
		VL = ch->count >> FSHIFT;
		ch->sample_pointer += VL;
		ch->count &= (FFIXED-1);

		if (ch->sample_pointer >= SOUND_DECODE_LENGTH)
		{
		    if (JUST_PLAYED_LAST_SAMPLE(ch))
		    {
			S9xAPUSetEndOfSample (J, ch);
			goto stereo_exit;
		    }
		    do
		    {
			ch->sample_pointer -= SOUND_DECODE_LENGTH;
			if (ch->last_block)
			{
			    if (!ch->loop)
			    {
				ch->sample_pointer = LAST_SAMPLE;
				break;
			    }
			    else
			    {
				S9xAPUSetEndX (J);
				ch->last_block = FALSE;
				unsigned short dir = S9xGetSampleAddress (ch->sample_number);
				ch->block_pointer = READ_WORD(dir + 2);
			    }
			}
			DecodeBlock (ch);
		    } while (ch->sample_pointer >= SOUND_DECODE_LENGTH);
		}
            }
	    else
	    {
	    }


	    short s;
	    if (ch->type == SOUND_SAMPLE) {
	      short vl = ((ch->count >> (FSHIFT-8))/*&255*/) - 256;
	      short * raw;
	      if (ch->sample_pointer > 15)
		raw = ch->decoded + 15;
	      else
		raw = ch->decoded + ch->sample_pointer;
	      long r;
	      r=((long)G4[-vl]*(long)*raw++);
	      r+=((long)G3[-vl]*(long)*raw++);
	      r+=((long)G2[vl]*(long)*raw++);
	      r+=((long)G1[vl]*(long)*raw++);
	      s = r >> 12;
	    } else {
	      for (;VL > 0; VL--)
		if ((so.noise_gen <<= 1) & 0x80000000L)
		  so.noise_gen ^= 0x0040001L;
	      s = so.noise_gen&0x7fff;
	    }
	    

	    if (mod2)
	      wave [I>>1] = (((((long) s * (long) ch->envx) >>7)+0x4000)/*&0x7fff*/);
	    /*else*/ {
	      VL = ((long) s * (long)ch->  left_vol_level) >>13;
	      VR = ((long) s * (long)ch-> right_vol_level) >>13;

	      MixBuffer [I  ] += VL;
	      MixBuffer [I+1] += VR;
	      ch->echo_buf_ptr [I  ] += VL;
	      ch->echo_buf_ptr [I+1] += VR;
	    }
        }
stereo_exit: ;
    }
}

#ifdef __sun
extern uint8 int2ulaw (int);
#endif

// For backwards compatibility with older port specific code
void S9xMixSamples (uint8 *buffer, int sample_count)
{
    S9xMixSamplesO (buffer, sample_count, 0);
}
#ifdef __DJGPP
END_OF_FUNCTION(S9xMixSamples);
#endif

void S9xMixSamplesO (uint8 * const restrict buffer, int sample_count, int byte_offset)
{
    int J;
    long I;
    static short Echo[SOUND_BUFFER_SIZE*2];
    int echoptr;

    if (so.mute_sound) return ;
    {
	memset (MixBuffer, 0, sample_count * sizeof (MixBuffer [0]));
	if (SoundData.echo_enable) {
	    memset (EchoBuffer, 0, sample_count * sizeof (EchoBuffer [0]));

#ifdef HAVE_ECHO	    
	    int offs = SoundData.echo_ptr;
	    int roffs = 0;
	    int size = sample_count;
 	    if (size > SoundData.echo_buffer_size)
 	      size = SoundData.echo_buffer_size;
	    dma_inuse = 1;
	    while (size > 0) {
	      while (dma_pending());
	      int n;
	      if (offs+size > SoundData.echo_buffer_size)
		n = SoundData.echo_buffer_size - offs;
	      else
		n = size;
	      dma_dsp2sdram(Echo+roffs, dsp_com->spc_echo_addr+(long)offs*2, (n*2), 0);
	      size -= n;
	      offs += n;
	      roffs += n;
	      if (offs >= SoundData.echo_buffer_size)
		offs -= SoundData.echo_buffer_size;
	    }
	    dma_inuse = 0;
	    while (dma_pending());
#endif
	}
	if (so.stereo)
	    MixStereo (sample_count);
	else
	    ;//MixMono (sample_count);
    }

    //prof(PROF_APU2);
    prof(3);

    BCOLOR(255, 0, 255);
    /* Mix and convert waveforms */
    if (so.sixteen_bit)
    {
	// 16-bit sound
	if (so.mute_sound)
	{
            memset (buffer, 0, sample_count);
	}
	else
	{
#ifndef HAVE_ECHO
	    if (0)
#else
	    if (SoundData.echo_enable && SoundData.echo_buffer_size)
#endif
	    {
		if (so.stereo)
		{
		    // 16-bit stereo sound with echo enabled ...
		    if (SoundData.no_filter)
		    {
#if 1
			// ... but no filter defined.
		        //signed short other;
		        int v;
		        for (v=0; v<2; v++) {
			  const short mv = SoundData.master_volume [v];
			  const short ev = SoundData.echo_volume [v];
			  echoptr = v;
			  for (J = v; J < sample_count; J += 2)
			    {
#ifdef HAVE_ECHO
			      short E = Echo [/*SoundData.echo_ptr*/echoptr];
			      
// 			      I = (((long) E * (long) SoundData.echo_feedback) >>7) +
// 				EchoBuffer [J];
			      I = (M_31_16(E, SoundData.echo_feedback) >>7) +
				EchoBuffer [J];
			      CLIP16(I); // VP ?
			      Echo [/*SoundData.echo_ptr*/echoptr] = I;
			      
			      echoptr += 2;
			      if (echoptr >= SoundData.echo_buffer_size)
				echoptr = v;
			      
#endif
			      
// 			      I = ((long) MixBuffer [J] * 
// 				   (long) mv
// #ifdef HAVE_ECHO
// 				   +
// 				   (long) E * (long) ev
// #endif
// 				   ) >>7;
			      I = (M_31_16(MixBuffer [J], mv)
#ifdef HAVE_ECHO
				   +
				   M_31_16(E, ev)
#endif
				   ) >>7;
			      
			      CLIP16(I);
			      ((signed short *) buffer)[J] = I;
// 			    extern void fifo_one(uint32 v);
// 			    if (J&1)
// 			      fifo_one(I | (other<<16));
// 			    else
// 			      other = I;
			    }
			}
#endif
		    }
		    else
		    {
			// ... with filter defined.
		        //signed short other;
		        int n = sample_count / 2;
			int v;
			for (v=0; v<2; v++) {
			  extern short Loop[2][SOUND_BUFFER_SIZE+8];
			  short * restrict loop = Loop[v] + n - 1;
			  const short mv = SoundData.master_volume [v];
			  const short ev = SoundData.echo_volume [v];
			  echoptr = v;
			  for (J = v; J < sample_count; J += 2)
			  {
#ifdef HAVE_ECHO
			    long E;
			    short e = Echo [/*SoundData.echo_ptr*/echoptr];

			    *loop++ = e;
#if 1
			    int * restrict filt = FilterTaps;
			    E =  (long) e       * (long) *filt++;
#if 0
			    // incredibly, using the RPT opcode is slower (probably because the number of loop isn't great enough)
			    int rep = 7;
			    while (rep--) // force compiler to use RPT with MAC
			      E += (long) *loop++ * (long) *filt++;
#else
			    E += (long) *loop++ * (long) *filt++;
			    E += (long) *loop++ * (long) *filt++;
			    E += (long) *loop++ * (long) *filt++;
			    E += (long) *loop++ * (long) *filt++;
			    E += (long) *loop++ * (long) *filt++;
			    E += (long) *loop++ * (long) *filt++;
			    E += (long) *loop++ * (long) *filt++;
#endif
			    E >>= 7;
			    loop -= 9;
#else
			    E =  (long) e                    * (long) FilterTaps [0];
			    E += (long) Loop [(Z -  2) & 15] * (long) FilterTaps [1];
			    E += (long) Loop [(Z -  4) & 15] * (long) FilterTaps [2];
			    E += (long) Loop [(Z -  6) & 15] * (long) FilterTaps [3];
			    E += (long) Loop [(Z -  8) & 15] * (long) FilterTaps [4];
			    E += (long) Loop [(Z - 10) & 15] * (long) FilterTaps [5];
			    E += (long) Loop [(Z - 12) & 15] * (long) FilterTaps [6];
			    E += (long) Loop [(Z - 14) & 15] * (long) FilterTaps [7];
			    E >>= 7;
			    Z++;
#endif

//  			    I = (((long) E * (long) SoundData.echo_feedback) >>7) +
//  							EchoBuffer [J];
 			    I = (M_31_16(E , SoundData.echo_feedback) >>7) +
			      EchoBuffer [J];
			    CLIP16(I); // VP ?
			    Echo [/*SoundData.echo_ptr*/echoptr] = I;

			    echoptr += 2;
			    if (echoptr >= SoundData.echo_buffer_size)
				echoptr = v;
#endif

// 			    I = ((long) MixBuffer [J] * 
// 				 (long) mv
// #ifdef HAVE_ECHO
// 				 +
// 				 (long) E * (long) ev
// #endif
// 				 ) >>7;
			    I = (M_31_16(MixBuffer [J], mv)
#ifdef HAVE_ECHO
				 +
				 M_31_16(E, ev)
#endif
				 ) >>7;

			    CLIP16(I);
			    ((signed short *) buffer)[J] = I;
// 			    extern void fifo_one(uint32 v);
// 			    if (J&1)
// 			      fifo_one(I | (other<<16));
// 			    else
// 			      other = I;
			  }
			  memcpy(Loop[v] + n, Loop[v], 8*sizeof(Loop[0][0]));
			}
		    }
		}
		else
		{
		}
	    }
	    else
	    {
#if 1
		// 16-bit mono or stereo sound, no echo
	        int v;
		for (v=0; v<2; v++) {
		  const short mv = SoundData.master_volume [v];
		  for (J = v; J < sample_count; J += 2)
		    {
// 		      I = ((long) MixBuffer [J] * 
// 			   (long) mv) >>7;
		      I = M_31_16((long) MixBuffer [J], mv) >>7;
		      
		      CLIP16(I);
// 		    if (J>sample_count/2)
// 		      I = 0x4000;
// 		    else
// 		      I = -0x4000;
		      ((signed short *) buffer)[J] = I;
		    }
		}
#endif
	    }
	}
    }
    else
    {
    }
    BCOLOR(255, 255, 0);

#ifdef HAVE_ECHO	    
	if (SoundData.echo_enable) {
	    int offs = SoundData.echo_ptr;
	    int roffs = 0;
	    int size = sample_count;
 	    if (size > SoundData.echo_buffer_size)
 	      size = SoundData.echo_buffer_size;
	    dma_inuse = 1;
	    while (size > 0) {
	      while (dma_pending());
	      int n;
	      if (offs+size > SoundData.echo_buffer_size)
		n = SoundData.echo_buffer_size - offs;
	      else
		n = size;
	      dma_dsp2sdram(Echo+roffs, dsp_com->spc_echo_addr+(long)offs*2, (n*2), 1);
	      size -= n;
	      offs += n;
	      roffs += n;
	      if (offs >= SoundData.echo_buffer_size)
		offs -= SoundData.echo_buffer_size;
	    }
	    dma_inuse = 0;
	    SoundData.echo_ptr = offs;
	}
#endif
}

#ifdef __DJGPP
END_OF_FUNCTION(S9xMixSamplesO);
#endif

void S9xResetSound (bool8 full)
{
    for (int i = 0; i < 8; i++)
    {
	SoundData.channels[i].state = SOUND_SILENT;
	SoundData.channels[i].mode = MODE_NONE;
	SoundData.channels[i].type = SOUND_SAMPLE;
	SoundData.channels[i].volume_left = 0;
	SoundData.channels[i].volume_right = 0;
	SoundData.channels[i].hertz = 0;
	SoundData.channels[i].count = 0;
	SoundData.channels[i].loop = FALSE;
	SoundData.channels[i].envx_target = 0;
	SoundData.channels[i].env_error = 0;
	SoundData.channels[i].erate = 0;
	SoundData.channels[i].envx = 0;
	SoundData.channels[i].envxx = 0;
	SoundData.channels[i].left_vol_level = 0;
	SoundData.channels[i].right_vol_level = 0;
	SoundData.channels[i].direction = 0;
	SoundData.channels[i].attack_rate = 0;
	SoundData.channels[i].decay_rate = 0;
	SoundData.channels[i].sustain_rate = 0;
	SoundData.channels[i].release_rate = 0;
	SoundData.channels[i].sustain_level = 0;
	SoundData.channels[i].latch_noise = 0;
	SoundData.echo_ptr = 0;
	SoundData.echo_feedback = 0;
	SoundData.echo_buffer_size = 2;
    }
    FilterTaps [0] = 127;
    FilterTaps [1] = 0;
    FilterTaps [2] = 0;
    FilterTaps [3] = 0;
    FilterTaps [4] = 0;
    FilterTaps [5] = 0;
    FilterTaps [6] = 0;
    FilterTaps [7] = 0;
    so.mute_sound = TRUE;
    so.noise_gen = 1;
    so.sound_switch = 255;
    so.samples_mixed_so_far = 0;
    so.play_position = 0;
    so.err_counter = 0;
    if (so.playback_rate)
	so.err_rate = (uint32) (FIXED_POINT * SNES_SCANLINE_TIME / (1.0 / so.playback_rate));
    else
	so.err_rate = 0;
    SoundData.no_filter = TRUE;
}

void S9xSetPlaybackRate (uint32 playback_rate)
{
    so.playback_rate = playback_rate;
    so.err_rate = (uint32) (SNES_SCANLINE_TIME * FIXED_POINT / (1.0 / (double) so.playback_rate));
    S9xSetEchoDelay (APU.DSP [APU_EDL] & 0xf);
    for (int i = 0; i < 8; i++)
	S9xSetSoundFrequency (i, SoundData.channels [i].hertz);
}

#if 0
bool8 S9xInitSound (int mode, bool8 stereo, int buffer_size)
{
    so.sound_fd = -1;
    so.sound_switch = 255;

    so.playback_rate = 0;
    so.buffer_size = 0;
    so.stereo = FALSE;
    so.sixteen_bit = FALSE;
    so.encoded = FALSE;
    
    S9xResetSound (TRUE);
    
    if (!(mode & 7))
	return (1);

    S9xSetSoundMute (TRUE);
    if (!S9xOpenSoundDevice (mode, stereo, buffer_size))
    {
	S9xMessage (S9X_ERROR, S9X_SOUND_DEVICE_OPEN_FAILED,
		    "Sound device open failed");
	return (0);
    }

    return (1);
}
#endif

bool8 S9xSetSoundMode (int channel, int mode)
{
    Channel *ch = &SoundData.channels[channel];

    switch (mode)
    {
    case MODE_RELEASE:
	if (ch->mode != MODE_NONE)
	{
	    ch->mode = MODE_RELEASE;
	    return (TRUE);
	}
	break;
	
    case MODE_DECREASE_LINEAR:
    case MODE_DECREASE_EXPONENTIAL:
    case MODE_GAIN:
	if (ch->mode != MODE_RELEASE)
	{
	    ch->mode = mode;
	    if (ch->state != SOUND_SILENT)
		ch->state = mode;

	    return (TRUE);
	}
	break;

    case MODE_INCREASE_LINEAR:
    case MODE_INCREASE_BENT_LINE:
	if (ch->mode != MODE_RELEASE)
	{
	    ch->mode = mode;
	    if (ch->state != SOUND_SILENT)
		ch->state = mode;

	    return (TRUE);
	}
	break;

    case MODE_ADSR:
	if (ch->mode == MODE_NONE || ch->mode == MODE_ADSR)
	{
	    ch->mode = mode;
	    return (TRUE);
	}
    }

    return (FALSE);
}

void S9xSetSoundControl (int sound_switch)
{
    so.sound_switch = sound_switch;
}

void S9xPlaySample (int channel)
{
    Channel *ch = &SoundData.channels[channel];
    
    ch->state = SOUND_SILENT;
    ch->mode = MODE_NONE;
    ch->envx = 0;
    ch->envxx = 0;

    S9xFixEnvelope (channel,
		    APU.DSP [APU_GAIN  + (channel << 4)], 
		    APU.DSP [APU_ADSR1 + (channel << 4)],
		    APU.DSP [APU_ADSR2 + (channel << 4)]);

    ch->sample_number = APU.DSP [APU_SRCN + channel * 0x10];
    ch->latch_noise = FALSE;
    if (APU.DSP [APU_NON] & (1 << channel))
	ch->type = SOUND_NOISE;
    else
	ch->type = SOUND_SAMPLE;

    S9xSetSoundFrequency (channel, ch->hertz);
    ch->loop = FALSE;
    ch->needs_decode = TRUE;
    ch->last_block = FALSE;
    memset(ch->decoded+15, 0, 4);
    unsigned short dir = S9xGetSampleAddress (ch->sample_number);
    ch->block_pointer = READ_WORD (dir);
    ch->sample_pointer = 0;
    ch->env_error = 0;

    switch (ch->mode)
    {
    case MODE_ADSR:
	if (ch->attack_rate == 0)
	{
	    if (ch->decay_rate == 0 || ch->sustain_level == 8)
	    {
		ch->state = SOUND_SUSTAIN;
		ch->envx = (MAX_ENVELOPE_HEIGHT * ch->sustain_level) >> 3;
		S9xSetEnvRate (ch, ch->sustain_rate, -1, 0);
	    }
	    else
	    {
		ch->state = SOUND_DECAY;
		ch->envx = MAX_ENVELOPE_HEIGHT;
		S9xSetEnvRate (ch, ch->decay_rate, -1, 
				    (MAX_ENVELOPE_HEIGHT * ch->sustain_level) >> 3);
	    }
	    ch-> left_vol_level = (ch->envx * ch->volume_left) /*/ 128*/;
	    ch->right_vol_level = (ch->envx * ch->volume_right) /*/ 128*/;
	}
	else
	{
	    ch->state = SOUND_ATTACK;
	    ch->envx = 0;
	    ch->left_vol_level = 0;
	    ch->right_vol_level = 0;
	    S9xSetEnvRate (ch, ch->attack_rate, 1, MAX_ENVELOPE_HEIGHT);
	}
	ch->envxx = (long)ch->envx << ENVX_SHIFT;
	break;

    case MODE_GAIN:
	ch->state = SOUND_GAIN;
	break;

    case MODE_INCREASE_LINEAR:
	ch->state = SOUND_INCREASE_LINEAR;
	break;

    case MODE_INCREASE_BENT_LINE:
	ch->state = SOUND_INCREASE_BENT_LINE;
	break;

    case MODE_DECREASE_LINEAR:
	ch->state = SOUND_DECREASE_LINEAR;
	break;

    case MODE_DECREASE_EXPONENTIAL:
	ch->state = SOUND_DECREASE_EXPONENTIAL;
	break;

    default:
	break;
    }

    S9xFixEnvelope (channel,
		    APU.DSP [APU_GAIN  + (channel << 4)], 
		    APU.DSP [APU_ADSR1 + (channel << 4)],
		    APU.DSP [APU_ADSR2 + (channel << 4)]);
}
