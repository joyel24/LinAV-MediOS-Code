/* 
*   include/sound.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SOUND_H
#define __SOUND_H

#include <kernel/sound.h>

#ifndef MAS_SET
#define MAS_SET                   0x00
#define MAS_GET                   0x01
#endif

/*********************** DSP   *******/

#define dsp_ini_mp3(ARG)                       {          API_DSP(DSP_INI_MP3        , (void*) ARG  ); }
#define dsp_start_mp3()                        {          API_DSP(DSP_START_MP3      , NULL         ); }
#define dsp_stop_mp3()                         {          API_DSP(DSP_STOP_MP3       , NULL         ); }
#define dsp_pause_mp3()                        {          API_DSP(DSP_PAUSE_MP3      , NULL         ); }
#define dsp_frame_cnt()                        { int val; API_DSP(DSP_FRAME_CNT      , (void*) &val ); }
#define dsp_in_peak(ARG)                       {          API_DSP(DSP_IN_PEAK        , (void*) ARG  ); }
#define dsp_out_peak(ARG)                      {          API_DSP(DSP_OUT_PEAK       , (void*) ARG  ); }
#define dsp_in_peak_real(ARG)                  {          API_DSP(DSP_IN_PEAK_REAL   , (void*) ARG  ); }
#define dsp_out_peak_real(ARG)                 {          API_DSP(DSP_OUT_PEAK_REAL  , (void*) ARG  ); }

/*********************** MIXER *******/

#define set_mixer_volume(val)                  { API_MIXER(MIXER_VOLUME     , MAS_SET , (void*)&val); }
#define set_mixer_balance(val)                 { API_MIXER(MIXER_BALANCE    , MAS_SET , (void*)&val); }
#define set_mixer_mute(val)                    { API_MIXER(MIXER_MUTE       , MAS_SET , (void*)&val); }
#define set_mixer_bass(val)                    { API_MIXER(MIXER_BASS       , MAS_SET , (void*)&val); }
#define set_mixer_treble(val)                  { API_MIXER(MIXER_TREBLE     , MAS_SET , (void*)&val); }
#define set_mixer_loudness(val)                { API_MIXER(MIXER_LOUDNESS   , MAS_SET , (void*)&val); }
#define set_mixer_mic_gain(val)                { API_MIXER(MIXER_MIC_GAIN   , MAS_SET , (void*)&val); }
#define set_mixer_adc_l_gain(val)              { API_MIXER(MIXER_ADC_L_GAIN , MAS_SET , (void*)&val); }
#define set_mixer_adc_r_gain(val)              { API_MIXER(MIXER_ADC_R_GAIN , MAS_SET , (void*)&val); }

#define get_mixer_volume()                     { int val; API_MIXER(MIXER_VOLUME     , MAS_GET , (void*)&val); val; }
#define get_mixer_balance()                    { int val; API_MIXER(MIXER_BALANCE    , MAS_GET , (void*)&val); val; }
#define get_mixer_mute()                       { int val; API_MIXER(MIXER_MUTE       , MAS_GET , (void*)&val); val; }
#define get_mixer_bass()                       { int val; API_MIXER(MIXER_BASS       , MAS_GET , (void*)&val); val; }
#define get_mixer_treble()                     { int val; API_MIXER(MIXER_TREBLE     , MAS_GET , (void*)&val); val; }
#define get_mixer_loudness()                   { int val; API_MIXER(MIXER_LOUDNESS   , MAS_GET , (void*)&val); val; }
#define get_mixer_mic_gain()                   { int val; API_MIXER(MIXER_MIC_GAIN   , MAS_GET , (void*)&val); val; }
#define get_mixer_adc_l_gain()                 { int val; API_MIXER(MIXER_ADC_L_GAIN , MAS_GET , (void*)&val); val; }
#define get_mixer_adc_r_gain()                 { int val; API_MIXER(MIXER_ADC_R_GAIN , MAS_GET , (void*)&val); val; }

#endif
