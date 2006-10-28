/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  System interface for sound.
 *
 *-----------------------------------------------------------------------------
 */

#include "medios.h"

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "m_swap.h"
#include "d_main.h"
#include "doomdef.h"

// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.

#define NUM_CHANNELS    16
// It is 2 for 16bit, and 2 for two channels.
#define BUFMUL          2
#define MIXBUFFERSIZE  (SAMPLECOUNT*BUFMUL)

#define SAMPLERATE  16000

// The global mixing buffer.
//  Basically, samples from all active internal channels
//  are modifed and added, and stored in the buffer
//  that is submitted to the audio device.
signed short mixbuffer[MIXBUFFERSIZE];
/* Don't place this in IRAM!
 * Sound playback uses DMA, and not all IRAM is DMA capable on coldfire. */

typedef struct {
   // SFX id of the playing sound effect.
   // Used to catch duplicates (like chainsaw).
   int id;
   // The channel step amount...
   unsigned int step;
   // ... and a 0.16 bit remainder of last step.
   unsigned int stepremainder;
   unsigned int samplerate;
   // The channel data pointers, start and end.
   const unsigned char* data;
   const unsigned char* enddata;
   // Time/gametic that the channel started playing,
   //  used to determine oldest, which automatically
   //  has lowest priority.
   // In case number of active sounds exceeds
   //  available channels.
   int starttime;
   // Hardware left and right channel volume lookup.
   int *leftvol_lookup;
   int *rightvol_lookup;
} channel_info_t;

channel_info_t channelinfo[NUM_CHANNELS];

int *vol_lookup; // Volume lookups.

int   steptable[256]; // Pitch to stepping lookup. (Not setup properly right now)


//*****************************************************************************
//*****************************************************************************

void I_UpdateSound( void );

tDspCom * dspCom;

__IRAM_CODE void dsp_interrupt(int irq,struct pt_regs * regs){
    // debug message coming from the dsp
    if (dspCom->hasDbgMsg){
        char * str=malloc(256);
        int i;

        for(i=0;i<256;++i){
            str[i]=dspCom->dbgMsg[i];
        }
        printf("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
        free(str);
    }

    if (dspCom->sndWantBuf){
        dspCom->armBusy=1;

        I_UpdateSound();

        dspCom->armBusy=0;
        dspCom->sndWantBuf=0;
    }
};

void dsp_init(){
    extern char _binary_apps_doom_dspcode_doom_dsp_out_start;
    extern char _binary_apps_doom_dspcode_doom_dsp_out_end;
    unsigned char * dspcode=&_binary_apps_doom_dspcode_doom_dsp_out_start;
    int len=&_binary_apps_doom_dspcode_doom_dsp_out_end-&_binary_apps_doom_dspcode_doom_dsp_out_start;

    // dsp irq handler
    irq_changeHandler(IRQ_DSP,dsp_interrupt);
    irq_enable(IRQ_DSP);

    *DSP_COM=0;
#if 1
    dsp_loadProgramFromMemory(dspcode,len);
#else
    dsp_loadProgramFromHDD("/doom_dsp.out");
#endif

    // setup dma
#ifdef DM270
    outw((inw(0x309a8)&0x03ff)|0x1000,0x309a8); //REFCTL : DMA 2 is used for DSP<>SDRAM
#endif

    dsp_run();

    while(!(*DSP_COM)); // wait for the dsp to finish init

    dspCom = (tDspCom *) DSP_RAM(*DSP_COM);

    dspCom->chipNum=dsp_getChipNum();

    dsp_write32(&dspCom->sndBufAddr,((unsigned long)mixbuffer)-SDRAM_START);

    dspCom->armBusy=0;
    dspCom->armInitFinished=1;
};


void snd_init(){
#ifdef HAVE_AIC23_SOUND
    aic23_setSampleRate(SAMPLERATE);
    aic23_enableOutput(true);
    aic23_setOutputVolume(AIC23_MAX_OUTPUT_VOLUME,AIC23_CHANNEL_BOTH);
#endif
}

void snd_close(){
#ifdef HAVE_AIC23_SOUND
    aic23_enableOutput(false);
#endif
}

//*****************************************************************************
//*****************************************************************************


//
// This function loads the sound data from the WAD lump for single sound.
// It is used to cache all the sounddata at startup.
//
void* getsfx( const char* sfxname )
{
   unsigned char*      sfx;
   unsigned char*      paddedsfx;
   int                 size;
   char                name[20];
   int                 sfxlump;

   // Get the sound data from the WAD, allocate lump
   //  in zone memory.
   snprintf(name, sizeof(name), "ds%s", sfxname);

   // Now, there is a severe problem with the sound handling, in it is not
   //  (yet/anymore) gamemode aware. That means, sounds from DOOM II will be
   //  requested even with DOOM shareware.
   // The sound list is wired into sounds.c, which sets the external variable.
   // I do not do runtime patches to that variable. Instead, we will use a
   //  default sound for replacement.
   if ( W_CheckNumForName(name) == -1 )
      sfxlump = W_GetNumForName("dspistol");
   else
      sfxlump = W_GetNumForName(name);

   size = W_LumpLength( sfxlump );

   sfx = (unsigned char*)W_CacheLumpNum( sfxlump, PU_STATIC);

   paddedsfx = (unsigned char*)malloc( size );  // Allocate from memory.
   memcpy(paddedsfx, sfx, size );               // Now copy and pad.

   return (void *) (paddedsfx); // Return allocated data.
}

/* cph
 * stopchan
 * Stops a sound
 */
static void stopchan(int i)
{
    channelinfo[i].data=NULL;
}

//
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
//
__IRAM_CODE int addsfx( int  sfxid, int  channel)
{
   stopchan(channel);

   // We will handle the new SFX.
   // Set pointer to raw data.
   {
      int lump = S_sfx[sfxid].lumpnum;
      size_t len = W_LumpLength(lump);

      /* Find padded length */
      len -= 8;
      channelinfo[channel].data = S_sfx[sfxid].data;

      /* Set pointer to end of raw data. */
      channelinfo[channel].enddata = channelinfo[channel].data + len - 1;
      channelinfo[channel].samplerate = (channelinfo[channel].data[3]<<8)+channelinfo[channel].data[2];
      channelinfo[channel].data += 8; /* Skip header */
   }

   channelinfo[channel].stepremainder = 0;
   // Should be gametic, I presume.
   channelinfo[channel].starttime = gametic;

    // Preserve sound SFX id,
    //  e.g. for avoiding duplicates of chainsaw.
   channelinfo[channel].id = sfxid;

   return channel;
}

__IRAM_CODE static void updateSoundParams(int handle, int volume, int seperation, int pitch)
{
   int rightvol;
   int leftvol;
   int slot = handle;
#if 0
   int step = steptable[pitch];
#endif

#ifdef RANGECHECK
   if (handle>=NUM_CHANNELS)
      I_Error("I_UpdateSoundParams: handle out of range");
#endif
   // Set stepping
   // MWM 2000-12-24: Calculates proportion of channel samplerate
   // to global samplerate for mixing purposes.
   // Patched to shift left *then* divide, to minimize roundoff errors
   // as well as to use SAMPLERATE as defined above, not to assume 11025 Hz
#if 0
   if (pitched_sounds)
      channelinfo[slot].step = step + (((channelinfo[slot].samplerate<<16)/SAMPLERATE)-65536);
   else
#endif
      channelinfo[slot].step = ((channelinfo[slot].samplerate<<16)/SAMPLERATE);

   // Separation, that is, orientation/stereo.
   //  range is: 1 - 256
   seperation += 1;

   // Per left/right channel.
   //  x^2 seperation,
   //  adjust volume properly.
   leftvol = volume - ((volume*seperation*seperation) >> 16);
   seperation = seperation - 257;
   rightvol= volume - ((volume*seperation*seperation) >> 16);

   // Sanity check, clamp volume.
   if (rightvol < 0 || rightvol > 127)
      I_Error("rightvol out of bounds");

   if (leftvol < 0 || leftvol > 127)
      I_Error("leftvol out of bounds");

   // Get the proper lookup table piece
   //  for this volume level???
   channelinfo[slot].leftvol_lookup = &vol_lookup[leftvol*256];
   channelinfo[slot].rightvol_lookup = &vol_lookup[rightvol*256];
}

void I_UpdateSoundParams(int handle, int volume, int seperation, int pitch)
{
  updateSoundParams(handle, volume, seperation, pitch);
}

//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
void I_SetChannels()
{
   // Init internal lookups (raw data, mixing buffer, channels).
   // This function sets up internal lookups used during
   //  the mixing process.
   int  i;
   int  j;
   int*  steptablemid = steptable + 128;

   // Okay, reset internal mixing channels to zero.
   for (i=0; i<NUM_CHANNELS; i++)
      memset(&channelinfo[i],0,sizeof(channel_info_t));

   // This table provides step widths for pitch parameters.
   for (i=-128 ; i<128 ; i++)
      steptablemid[i]=2;
//      steptablemid[i] = (int)(pow(1.2, ((double)i/(64.0*SAMPLERATE/11025)))*65536.0);

   // Generates volume lookup tables
   //  which also turn the unsigned samples
   //  into signed samples.
   for (i=0 ; i<128 ; i++)
      for (j=0 ; j<256 ; j++)
         vol_lookup[i*256+j] = 3*(i*(j-128)*256)/191;
}

void I_SetSfxVolume(int volume)
{
   // Identical to DOS.
   // Basically, this should propagate
   //  the menu/config file setting
   //  to the state variable used in
   //  the mixing.
   snd_SfxVolume = volume;
}

// MUSIC API - dummy. Some code from DOS version.
void I_SetMusicVolume(int volume)
{
   // Internal state variable.
   snd_MusicVolume = volume;
   // Now set volume on output device.
   // Whatever( snd_MusciVolume );
}

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
   char namebuf[9];
   snprintf(namebuf, sizeof(namebuf), "ds%s", sfx->name);
   return W_GetNumForName(namebuf);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//

int next_channel=0;

__IRAM_CODE int I_StartSound(int id, int vol, int sep, int pitch, int priority)
{
   (void)priority;
   int handle;

   //gli
   int channel=next_channel;
   next_channel=(next_channel+1)%NUM_CHANNELS;

   while(dspCom->armBusy || dspCom->sndWantBuf) /* wait for ready */;
   dspCom->armBusy=1;

   // Returns a handle (not used).
   handle = addsfx(id,channel);

#ifdef RANGECHECK
   if (handle>=NUM_CHANNELS)
      I_Error("I_StartSound: handle out of range");
#endif
   updateSoundParams(handle, vol, sep, pitch);

   dspCom->armBusy=0;

   return handle;
}

void I_StopSound (int handle)
{
#ifdef RANGECHECK
   if (handle>=NUM_CHANNELS)
      I_Error("I_StopSound: handle out of range");
#endif
   stopchan(handle);
}

int I_SoundIsPlaying(int handle)
{
#ifdef RANGECHECK
   if (handle>=NUM_CHANNELS)
      I_Error("I_SoundIsPlaying: handle out of range");
#endif
   return channelinfo[handle].data != NULL;
}

//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the given
//  mixing buffer, and clamping it to the allowed
//  range.
//
// This function currently supports only 16bit.
//

__IRAM_CODE void I_UpdateSound( void )
{
   // Mix current sound data.
   // Data, from raw sound, for right and left.
   register unsigned char sample;
   register int  dl;
   register int  dr;

   // Pointers in global mixbuffer, left, right, end.
   signed short*  leftout;
   signed short*  rightout;
   signed short*  leftend;

   // Step in mixbuffer, left and right, thus two.
   int    step;

   // Mixing channel index.
   int    chan;

   // Left and right channel
   //  are in global mixbuffer, alternating.
   leftout = mixbuffer;
   rightout = mixbuffer +1;
   step = 2;

   // Determine end, for left channel only
   //  (right channel is implicit).
   leftend = mixbuffer + SAMPLECOUNT*step;

   // Mix sounds into the mixing buffer.
   // Loop over step*SAMPLECOUNT,
   //  that is 512 values for two channels.
   while (leftout != leftend)
   {
      // Reset left/right value.
      dl = 0;
      dr = 0;

      // Love thy L2 chache - made this a loop.
      // Now more channels could be set at compile time
      //  as well. Thus loop those  channels.
      for ( chan = 0; chan < NUM_CHANNELS; chan++ )
      {
         // Check channel, if active.
         if (channelinfo[chan].data)
         {
            // Get the raw data from the channel.
            sample = (((unsigned int)channelinfo[chan].data[0] * (0x10000 - channelinfo[chan].stepremainder))
                     + ((unsigned int)channelinfo[chan].data[1] * (channelinfo[chan].stepremainder))) >> 16;
            // Add left and right part
            //  for this channel (sound)
            //  to the current data.
            // Adjust volume accordingly.
            dl += channelinfo[chan].leftvol_lookup[sample];
            dr += channelinfo[chan].rightvol_lookup[sample];
            // Increment index ???
            channelinfo[chan].stepremainder += channelinfo[chan].step;
            // MSB is next sample???
            channelinfo[chan].data += channelinfo[chan].stepremainder >> 16;
            // Limit to LSB???
            channelinfo[chan].stepremainder &= 0xffff;

            // Check whether we are done.
            if (channelinfo[chan].data >= channelinfo[chan].enddata)
               stopchan(chan);
         }
      }

      // Clamp to range. Left hardware channel.
      // Has been char instead of short.
      // if (dl > 127) *leftout = 127;
      // else if (dl < -128) *leftout = -128;
      // else *leftout = dl;

      if (dl > 0x7fff)
         *leftout = 0x7fff;
      else if (dl < -0x8000)
         *leftout = -0x8000;
      else
         *leftout = (signed short)dl;

      // Same for right hardware channel.
      if (dr > 0x7fff)
         *rightout = 0x7fff;
      else if (dr < -0x8000)
         *rightout = -0x8000;
      else
         *rightout = (signed short)dr;

      // Increment current pointers in mixbuffer.
      leftout += step;
      rightout += step;
   }
}

//
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime.
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//

void get_more(unsigned char** start, size_t* size)
{
   I_UpdateSound(); // Force sound update

   *start = (unsigned char*)(mixbuffer);
   *size = SAMPLECOUNT*2*sizeof(short);
}


void I_SubmitSound(void)
{
      return;
}

void I_ShutdownSound(void)
{
    snd_close();
    dsp_reset();
    dsp_off();
}

void I_InitSound()
{
   int i;

   // Initialize external data (all sounds) at start, keep static.
   printf( "I_InitSound: ");

   vol_lookup=malloc(128*256*sizeof(int));

   for (i=1 ; i<NUMSFX ; i++)
   {
      if (!S_sfx[i].link) // Alias? Example is the chaingun sound linked to pistol.
         S_sfx[i].data = getsfx( S_sfx[i].name); // Load data from WAD file.
      else
         S_sfx[i].data = S_sfx[i].link->data; // Previously loaded already?
   }

   printf( " pre-cached all sound data\n");

   // Now initialize mixbuffer with zero.
   for ( i = 0; i< MIXBUFFERSIZE; i++ )
      mixbuffer[i] = 0;

   snd_init();

// for now dsp only supports AIC23
#ifdef HAVE_AIC23_SOUND
   dsp_init();
#endif

   // Finished initialization.
   printf("I_InitSound: sound module ready\n");
}

//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic(void)  {
}
void I_ShutdownMusic(void) {
}

static int looping=0;
static int musicdies=-1;

void I_PlaySong(int handle, int looping)
{
   // UNUSED.
   handle = looping = 0;
   musicdies = gametic + TICRATE*30;
}

void I_PauseSong (int handle)
{
   // UNUSED.
   handle = 0;
}

void I_ResumeSong (int handle)
{
   // UNUSED.
   handle = 0;
}

void I_StopSong(int handle)
{
   // UNUSED.
   handle = 0;

   looping = 0;
   musicdies = 0;
}

void I_UnRegisterSong(int handle)
{
   // UNUSED.
   handle = 0;
}

int I_RegisterSong(void *data)
{
   // UNUSED.
   data = NULL;

   return 1;
}

// Is the song playing?
int I_QrySongPlaying(int handle)
{
   // UNUSED.
   handle = 0;
   return looping || musicdies > gametic;
}

// Interrupt handler.
void I_HandleSoundTimer( int ignore )
{
   (void)ignore;
}

// Get the interrupt. Set duration in millisecs.
int I_SoundSetTimer( int duration_of_tick )
{
   (void)duration_of_tick;
   // Error is -1.
   return 0;
}

// Remove the interrupt. Set duration to zero.
void I_SoundDelTimer(void)
{
}
