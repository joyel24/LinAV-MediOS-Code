/* 
*   include/mas.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SOUND_H
#define __SOUND_H

#include <kernel/mas.h>

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

/********************* DSP                    ***************************/
/* dev functions */
void    dsp_interrupt     (int irq);
void    dsp_ctl           (unsigned int cmd,int dir, void * arg);

/* mp3 functions */
void mp3_read_more(void);
int start_mp3_playback(void);
int stop_mp3_playback(void);
int ini_mp3_playback(struct mp3_play * arg);
int ini_mas_for_mp3(void);

/* line in functions */

void mas_line_in_on(void);
void mas_line_in_off(void);

/********************* MIXER                  ***************************/
void mixer_ctl(unsigned int cmd, int dir, void * arg);

/********************* OSS init               ***************************/
void init_sound(void);

#endif

