#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_dma.h>

#include <libdsp.h>

#include "../dspshared.h"
#include "../nes_pal.h"
#include "nes_apu.h"

// seems to be the best values for speed and sound quality
#define CHUNK_HEIGHT 5
#define SNDBUF_LENGTH 20

unsigned short sndBuf[SNDBUF_LENGTH];
unsigned short sndBuf2[SNDBUF_LENGTH];
unsigned short sndBufNum;

unsigned short inBuf[NES_BUFFER_WIDTH*CHUNK_HEIGHT];

unsigned long outBuf[NES_WIDTH*CHUNK_HEIGHT];
unsigned long outBuf2[NES_WIDTH*CHUNK_HEIGHT];

unsigned long framePal[32];

tDspCom dspComBuffer;
tDspCom * dspCom;

void debug(const char* msg);
void handleVideoBuffer();
void doAicDma(unsigned short * buffer,unsigned short length);
short aic23_callback(void * buffer);

void main(){
	short i;

    *DSP_COM=0;

    // dsp<>arm comm
    dspCom = &dspComBuffer;
	memset((void *)dspCom,0,sizeof(dspCom));
    *DSP_COM = (unsigned short) dspCom;

	// wait for ARM to finish init
	while(!dspCom->armInitFinished);

	CSL_init();

	libDsp_init(dspCom->chipNum);

	ibDma_reset();

	// aic23
	if (aic23_openPort()==INV) debug("Data Port NOK");
	aic23_setupDma(SNDBUF_LENGTH,aic23_callback);	
	aic23_startDma();

    // nes apu
 	apu_create(SAMPLE_RATE, 60, 0, 16);
    apu_setfilter(APU_FILTER_NONE);

	// palette : swap cb and cr
	for(i=0;i<64;++i){
		nes_pal[i]= (nes_pal[i]/0x10000) + (nes_pal[i]*0x10000);
	}

	debug("DSP START");

	dspCom->dspInitFinished=1;

	// the main prog handles video, sound is done with dma+irq
	for(;;){
		if(dspCom->inBufReady){
#if 0
			dspCom->inBufReady=0;
#else
#ifdef DSP_VID_PROFILE
		    armInt_trigger();
#endif
			handleVideoBuffer();
#endif
		}
	}
}

void handleVideoBuffer(){
	unsigned long inSdAddr=dspCom->inBufAddr - SDRAM_OFFSET;
	unsigned long outSdAddr=dspCom->outBufAddr - SDRAM_OFFSET;
	unsigned short * inPtr;
	unsigned long * outPtr;
	short outBufNum=0;
	short line=0;
	short i,j,k;

	// build frame palette
	for(i=0;i<32;++i){
	    framePal[i]=nes_pal[dspCom->pal[i]];
	}

	line+=10;
	inSdAddr+=(unsigned long) NES_BUFFER_WIDTH*10;
	outSdAddr+=(unsigned long) NES_WIDTH*10*4;

	for(i=0;i<(NES_PAL_HEIGHT-20)/CHUNK_HEIGHT;++i){
		inPtr=inBuf;

		if (outBufNum==0){
			outPtr=outBuf;
		}else{
			outPtr=outBuf2;
		}

		ibDma_start(inSdAddr,NES_BUFFER_WIDTH*CHUNK_HEIGHT,
					0,NES_BUFFER_WIDTH*CHUNK_HEIGHT,
					NES_BUFFER_WIDTH*CHUNK_HEIGHT,1,
					IB_BUF_A,IB_DIR_SD2IB,1);
		while(ibDma_pending());
		ibDma_reset();

		memcpy(inBuf,IB_DM270_BUFA_ADDR,sizeof(inBuf));

		for(j=0;j<CHUNK_HEIGHT;++j){
			inPtr=inBuf+j*NES_BUFFER_WIDTH+dspCom->lineOffset[line];

			for (k=0;k<256;k++){
				*outPtr++=framePal[*inPtr++];
			}
			line++;
		}

		while(hpiDma_pending());
		if (outBufNum==0){
			hpiDma_start(outSdAddr,outBuf,sizeof(outBuf)*2,HPI_DIR_DSP2SD);
			outBufNum++;
		}else{
			hpiDma_start(outSdAddr,outBuf2,sizeof(outBuf2)*2,HPI_DIR_DSP2SD);
			outBufNum--;
		}

		inSdAddr+=(unsigned long) NES_BUFFER_WIDTH*CHUNK_HEIGHT;
		outSdAddr+=(unsigned long) NES_WIDTH*CHUNK_HEIGHT*4;
	}
	dspCom->inBufReady=0;
	dspCom->outBufReady=1;
    armInt_trigger();
}

short aic23_callback(void * buffer){
	apu_setfilter(dspCom->sndFilter);
	
	// handle pause & reset
	dspCom->sndIsPaused=dspCom->sndWantPause;
	if (dspCom->sndIsPaused) debug("sound paused");
	do{
		// handle reset
		if (dspCom->sndWantApuReset){
			debug("apu reset");
			apu_reset();
			dspCom->sndWantApuReset=0;
		}

		if (dspCom->sndIsPaused && !dspCom->sndWantPause){
			dspCom->sndIsPaused=0;
			debug("sound unpaused");
		}
	}while(dspCom->sndIsPaused);

	dspCom->sndStatusReg=apu_read(APU_SMASK);

	apu_process(buffer,SNDBUF_LENGTH/2);    	

	return 1;
}

void debug(const char* msg){
	if (!dspCom) return;

	strcpy((char*)dspCom->dbgMsg,msg);
	dspCom->hasDbgMsg=1;

    armInt_trigger();

	while(dspCom->hasDbgMsg);
}
