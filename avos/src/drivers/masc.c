// avOS - http://avos.sourceforge.net
// Copyright (c) 2003 by Jimmy Moore
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.
//
// Date:     04/01/2004
// Author:   By DoggerMoore

#include "mas.h"

//
int masGetInPeakLeft() {
    return masReadCodecRegA(0x000a) & 0x7fff;
}

//
int masGetInPeakRight() {
    return masReadCodecRegA(0x000b) & 0x7fff;
}

//
int masGetOutPeakLeft() {
    return masReadCodecRegA(0x000c) & 0x7fff;
}

//
int masGetOutPeakRight() {    
    return masReadCodecRegA(0x000d) & 0x7fff;
}

//
void masSetBalance(int b) {         // Note need to compensate with volume
    masWriteCodecRegA(0x0011, (b & 0xff) << 8); 
}

//
void masSetVolume(int v) {
    masWriteCodecRegA(0x0010, (v & 0xff) << 8); 
}

//
void masSetLoudness(int loudnessType, int l) {
    masWriteCodecRegA(0x001e, ((l & 0xff) << 8) | loudnessType); 
}

//
void masSetTreble(int t) {
    masWriteCodecRegA(0x0015, (t & 0xff) << 8); 
}

//
void masSetBass(int b) {
    masWriteCodecRegA(0x0014, (b & 0xff) << 8); 
}

//
void masConfigAudioCodec(int adcGainLeft, int adcGainRight, int micGain,
                         int config) {
    masWriteCodecRegA(0x0000, config
                              | (micGain << 4)
                              | (adcGainRight << 8)
                              | (adcGainLeft << 12));
}

//
void masConfigInput(int config) {
    masWriteCodecRegA(0x0008, config);    
}

//
void masSetDACMix(int mixadc, int mixdsp) {
    masWriteCodecRegA(0x0006, mixadc << 8);
    masWriteCodecRegA(0x0007, mixdsp << 8);
}
