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

/********************** DSP ctl                    **********************/

#define DSP_INI_MP3             0x0101
#define DSP_START_MP3           0x0102
#define DSP_STOP_MP3            0x0103
#define DSP_PAUSE_MP3           0x0104
#define DSP_FRAME_CNT           0x0105
#define DSP_IN_PEAK             0x0106
#define DSP_OUT_PEAK            0x0107
#define DSP_IN_PEAK_REAL        0x0108
#define DSP_OUT_PEAK_REAL       0x0109
#define DSP_SETCURR_MP3_BUFFER  0x010A
#define DSP_GETCURR_MP3_BUFFER  0x010B
#define DSP_ADD_MP3_BUFFER      0x010C
#define DSP_REMOVE_MP3_BUFFER   0x010D

/********************** mp3 player                 **********************/

struct mp3_play {
        int size;
        char * buffer;
        int endOfFile;
        int buffer_read;
        int buffer_write;
	int buffer_len;
	int finished;
	char * filename;
	int pos;
        int freqPeakDraw;
        void * (*peakDraw)(int left,int right,int frame);
};

struct av_peak {
	int left;
	int right;
};

/********************* MIXER ctl              ***************************/
#define MIXER_VOLUME            0x0001
#define MIXER_BALANCE           0x0002
#define MIXER_MUTE              0x0003
#define MIXER_BASS              0x0004
#define MIXER_TREBLE            0x0005
#define MIXER_LOUDNESS          0x0006
#define MIXER_MIC_GAIN          0x0007
#define MIXER_ADC_L_GAIN        0x0008
#define MIXER_ADC_R_GAIN        0x0009

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
