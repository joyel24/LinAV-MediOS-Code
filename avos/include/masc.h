/* libavos.h
   By William Bland (aka awksedgrep)
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/
#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

// 0-0x7fff values.
int masGetInPeakLeft();
int masGetInPeakRight();
int masGetOutPeakLeft();
int masGetOutPeakRight();

// signed byte. 0x7f =  left -127dB, right 0dB
//              0x00 =  left    0dB, right 0dB
//              0x80 =  left    0dB, right -128dB
void masSetBalance(int b);

// unsigned 0x00=mute, 0x01=-114dB, 0x7f=+12dB
void masSetVolume(int v);

#define MAS_LOUDNESS_NORMAL 0       // Constant vol at 1Khz
#define MAS_LOUDNESS_SUPERBASS 4    // Constant vol at 2khz

// unsigned 0x00=0dB, 0x44=+17dB
void masSetLoudness(int loudnessType, int l);

// signed byte. 0x60 = +12dB, 0xa0 = -12dB
void masSetTreble(int t);

// signed byte. 0x60 = +12dB, 0xa0 = -12dB
void masSetBass(int b);

#define MAS_CONFIG_INPUT_LINEIN    0
#define MAS_CONFIG_INPUT_MIC       8
#define MAS_CONFIG_ADCLEFT_ENABLE  4
#define MAS_CONFIG_ADCRIGHT_ENABLE 2
#define MAS_CONFIG_DAC_ENABLE      1
// gains are 4 bit.
// adc gains - 00=-3dB, 0f=+19.5dB
// mic gain  - 00=+21dB, 0f=+43.5dB
void masConfigAudioCodec(int adcGainLeft, int adcGainRight, int micGain, int config);

#define MAS_CONFIGINPUT_STEREO              0
#define MAS_CONFIGINPUT_MONO                0x8000
#define MAS_CONFIGINPUT_DEEMPHASIS_OFF      0
#define MAS_CONFIGINPUT_DEEMPHASIS_50us     1
#define MAS_CONFIGINPUT_DEEMPHASIS_75us     2
void masConfigInput(int config);

// 0x40 = 100%, 0x7f=200% clipping occurs if sum>100%
void masSetDACMix(int mixadc, int mixdsp);
