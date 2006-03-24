#include <stdio.h>
#include <kernel/dsp.h>

//#include "dspcode.h"
#include <sys_def/types.h>
#include "dspshared.h"

dsp_com_t * dsp_com;

/*

 All files in this archive are subject to the GNU General Public License.
 See the file COPYING in the source tree root for full license agreement.
 This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 KIND, either express of implied.

 AIC23 audio codec access

 Date:     01/11/2005
 Author:   GliGli

*/

#include <aic23.h>


void aic23WriteReg(int address,int value){
  char val;
  val=value;

  i2c_write(AIC23_I2C_ADDRESS,(address<<1)|(value>>8),&val,1);
}

void aic23Init(){
  aic23WriteReg(15,0x00); //reset
  aic23WriteReg(4,0x12); //analog audio path: dac selected & mic muted
  aic23WriteReg(5,0x00); //digital audio path: nothing
  aic23WriteReg(6,0x07); //power down control: activate everything but input
  aic23WriteReg(7,0x43); //digital audio interface: master mode & dsp data format

  // 10001 --> 44.1 Khz
  // 01100 --> 32   Khz
  //aic23WriteReg(8,1 + (23<<1)); //sample rate control: 32khz & oversampling & USB mode
  aic23WriteReg(8,1 + (0xc<<1)); //sample rate control: 32khz & oversampling & USB mode
  //aic23WriteReg(8,0x23); //sample rate control: 44khz & oversampling & USB mode

  aic23WriteReg(9,0x01); //activate digital interface
}

void aic23Shutdown(){
  aic23WriteReg(2,0x180); //output volume = 0
  aic23WriteReg(9,0x00); //deactivate digital interface
  aic23WriteReg(6,0xff); //power down control: shutdown everything
}

void aic23SetOutputVolume(int volume,int channel){
  switch (channel){
    case AIC23_CHANNEL_BOTH:
      aic23WriteReg(2,volume | 0x180);
      break;
    case AIC23_CHANNEL_LEFT:
      aic23WriteReg(2,volume | 0x80);
      break;
    case AIC23_CHANNEL_RIGHT:
      aic23WriteReg(3,volume | 0x80);
      break;
  }
}

void initCodec(){
  aic23Init();

  aic23SetOutputVolume(0,AIC23_CHANNEL_BOTH);

  //aic23SetOutputVolume(127,AIC23_CHANNEL_BOTH);

  aic23SetOutputVolume(127,AIC23_CHANNEL_LEFT);
  aic23SetOutputVolume(127,AIC23_CHANNEL_RIGHT);
/*   aic23SetOutputVolume(121,AIC23_CHANNEL_LEFT); */
/*   aic23SetOutputVolume(121,AIC23_CHANNEL_RIGHT); */
}

void initDSP()
{
  unsigned char *dspcode;
  int len;
  int wait;

/*   initCodec(); */

#if 0
  dspcode=(unsigned char *) chunks[0].chunk;
  len=chunks[0].size;
#else
  extern char _binary_apps_snes9x_dspcode_snes9x_dsp_out_start;
  extern char _binary_apps_snes9x_dspcode_snes9x_dsp_out_end;
  dspcode=&_binary_apps_snes9x_dspcode_snes9x_dsp_out_start;
  len=&_binary_apps_snes9x_dspcode_snes9x_dsp_out_end-&_binary_apps_snes9x_dspcode_snes9x_dsp_out_start;
#endif

  printf("initDSP() dspcode=%0.8X len=%d\n",dspcode,len);

  {
    char buf[] = "1234567890abcdef";
    memcpy(0xc10000 + 32*0x10000, buf, sizeof(buf));

    uint16_t * refctl = (uint16_t *) 0x309a8;
    *refctl = (*refctl & 0x03ff) | 0x8000;

/*     uint16_t * imgdspdest = (uint16_t *) 0x30a34; */
/*     *imgdspdest = 5; */
  }

  *DSP_COM=0;
  load_dsp_program_mem(dspcode,len);

  dsp_run();

  wait = 0;
  while(!(*DSP_COM)) {
     // wait for the dsp to finish init
    if (wait == 100000) {
      int i;
      for (i=0; i<10; i++) 
	printf("%c\n", ((char *)(0x00C10000 + 32*0x10000))[i]);
    }
    wait++;
  }

  dsp_com = (dsp_com_t *) DSP_RAM(*DSP_COM);

  {
    char * p;
    int i, j;
    char test[] = "world";

    for (j=0; j<8; j++) {
      printf("%c%c\n", DSP_RAM(dsp_com->hello)[j], DSP_RAM(dsp_com->hello)[j]>>8);
    }
    for (i=0; i<10; i++) 
      printf("%c\n", ((char *)(0x00C10000 + 32*0x10000))[i]);

    while (!(btn_readState() & 4));

/*     for (p=(char *)0x900000; p<((char *)0x900000) + 16*1024*1024; p++) { */
/*       for (j=0; j<sizeof(test)-1; j++) */
/* 	if (test[j] != p[j*2]) */
/* 	  break; */
/*       if (j == sizeof(test)-1) { */
/* 	printf("FOUND AT %x (%d) !!!!!\n", p, p - ((char *)0xc10000)); */
/* 	//break; */
/*       } */
/*     } */
  }

  initCodec();

}

