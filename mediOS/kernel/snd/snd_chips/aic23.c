/*
*   kernel/driver/aic23.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>
#include <sys_def/types.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/aic23.h>
#include <kernel/i2c.h>

typedef struct{
  int samplerate;
  int config;
}aic23_samplerate;

// supported sample rates and their corresponding config
static const aic23_samplerate aic23_samplerates[]={
  {4000, AIC23_REG_SRATE_HALF_INPUT_CLOCK|AIC23_REG_SRATE_HALF_OUTPUT_CLOCK|
  			   AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_3|AIC23_REG_SRATE_OVERSAMPLING_0},
  {4010, AIC23_REG_SRATE_HALF_INPUT_CLOCK|AIC23_REG_SRATE_HALF_OUTPUT_CLOCK|
  			   AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_11|AIC23_REG_SRATE_OVERSAMPLING_1},
  {8000, AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_3|AIC23_REG_SRATE_OVERSAMPLING_0},
  {8021, AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_11|AIC23_REG_SRATE_OVERSAMPLING_1},
  {16000,AIC23_REG_SRATE_HALF_INPUT_CLOCK|AIC23_REG_SRATE_HALF_OUTPUT_CLOCK|
  			   AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_6|AIC23_REG_SRATE_OVERSAMPLING_0},
  {22050,AIC23_REG_SRATE_HALF_INPUT_CLOCK|AIC23_REG_SRATE_HALF_OUTPUT_CLOCK|
  			   AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_8|AIC23_REG_SRATE_OVERSAMPLING_1},
  {24000,AIC23_REG_SRATE_HALF_INPUT_CLOCK|AIC23_REG_SRATE_HALF_OUTPUT_CLOCK|
  			   AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_0|AIC23_REG_SRATE_OVERSAMPLING_0},
  {32000,AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_6|AIC23_REG_SRATE_OVERSAMPLING_0},
  {44100,AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_8|AIC23_REG_SRATE_OVERSAMPLING_1},
  {48000,AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_0|AIC23_REG_SRATE_OVERSAMPLING_0},
  {88200,AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_15|AIC23_REG_SRATE_OVERSAMPLING_1},
  {96000,AIC23_REG_SRATE_CLOCK_MODE_USB|AIC23_REG_SRATE_SAMPLE_RATE_7|AIC23_REG_SRATE_OVERSAMPLING_0}
};

static int aic23_regValues[AIC23_NUM_REGS];

int aic23_readReg(int address){
  return aic23_regValues[address];
}

void aic23_writeReg(int address,int value){
  char val;
  val=value;

  aic23_regValues[address]=value;
  i2c_write(AIC23_I2C_ADDRESS,(address<<1)|(value>>8),&val,1);

//  printk("aic %d=%0.5x\n",address,value);
}

bool aic23_setSampleRate(int rate){
  int i;
  for(i=0;i<(sizeof(aic23_samplerates)/sizeof(aic23_samplerate));++i){
    if(rate==aic23_samplerates[i].samplerate){
      aic23_writeReg(AIC23_REG_SAMPLE_RATE,aic23_samplerates[i].config);
      return true;
    }
  }
  return false;
}

void aic23_setOutputVolume(int volume,int channel){
  int vol=volume;

  if (vol>AIC23_MAX_OUTPUT_VOLUME) vol=AIC23_MAX_OUTPUT_VOLUME;

  if(channel&AIC23_CHANNEL_LEFT){
  	aic23_writeReg(AIC23_REG_LEFT_OUTPUT_VOLUME,AIC23_REG_OUTVOL_ZERO_CROSS_DETECT|vol);
  };
  if(channel&AIC23_CHANNEL_RIGHT){
  	aic23_writeReg(AIC23_REG_RIGHT_OUTPUT_VOLUME,AIC23_REG_OUTVOL_ZERO_CROSS_DETECT|vol);
  };
}

void aic23_setInputVolume(int volume,int channel){
  int vol=volume;

  if (vol>AIC23_MAX_INPUT_VOLUME) vol=AIC23_MAX_INPUT_VOLUME;

  if(channel&AIC23_CHANNEL_LEFT){
  	aic23_writeReg(AIC23_REG_LEFT_INPUT_VOLUME,vol);
  };
  if(channel&AIC23_CHANNEL_RIGHT){
  	aic23_writeReg(AIC23_REG_RIGHT_INPUT_VOLUME,vol);
  };
}

void aic23_enableOutput(bool enable){
  if(enable){
    // power up output
    aic23_writeReg(AIC23_REG_POWER_DOWN,
      aic23_regValues[AIC23_REG_POWER_DOWN]&(~(AIC23_REG_PWRDN_OUTPUT|AIC23_REG_PWRDN_DAC)));
    // enable DAC output
  	aic23_writeReg(AIC23_REG_ANALOG_PATH,
      aic23_regValues[AIC23_REG_ANALOG_PATH]|AIC23_REG_ANAP_DAC_ENABLE);
    // remove soft mute
  	aic23_writeReg(AIC23_REG_DIGITAL_PATH,
      aic23_regValues[AIC23_REG_DIGITAL_PATH]&(~AIC23_REG_DIGP_DAC_SOFT_MUTE));
  }else{
    // set soft mute
  	aic23_writeReg(AIC23_REG_DIGITAL_PATH,
      aic23_regValues[AIC23_REG_DIGITAL_PATH]|AIC23_REG_DIGP_DAC_SOFT_MUTE);
    // disable DAC output
  	aic23_writeReg(AIC23_REG_ANALOG_PATH,
      aic23_regValues[AIC23_REG_ANALOG_PATH]&(~AIC23_REG_ANAP_DAC_ENABLE));
    // power down output
    aic23_writeReg(AIC23_REG_POWER_DOWN,
      aic23_regValues[AIC23_REG_POWER_DOWN]|AIC23_REG_PWRDN_OUTPUT|AIC23_REG_PWRDN_DAC);
  };
}

void aic23_enableInput(bool enable){
  if(enable){
    // power up input
    aic23_writeReg(AIC23_REG_POWER_DOWN,
      aic23_regValues[AIC23_REG_POWER_DOWN]&(~(AIC23_REG_PWRDN_ADC|AIC23_REG_PWRDN_MIC|AIC23_REG_PWRDN_LINEIN)));
  }else{
    // power down input
    aic23_writeReg(AIC23_REG_POWER_DOWN,
      aic23_regValues[AIC23_REG_POWER_DOWN]|AIC23_REG_PWRDN_ADC|AIC23_REG_PWRDN_MIC|AIC23_REG_PWRDN_LINEIN);
  };
}


void aic23_configureInput(int source,bool micboost){
  int value=aic23_regValues[AIC23_REG_ANALOG_PATH];

  if(source==AIC23_SOURCE_MIC){
    value|=AIC23_REG_ANAP_INPUT_USE_MIC;
    value&=~AIC23_REG_ANAP_MIC_MUTE;
  }else{
    value&=~AIC23_REG_ANAP_INPUT_USE_MIC; //AIC23_REG_ANAP_INPUT_USE_LINEIN=0 so I can't use it as a mask
    value|=AIC23_REG_ANAP_MIC_MUTE;
  };

  if (micboost){
    value|=AIC23_REG_ANAP_MIC_BOOST;
  }else{
    value&=~AIC23_REG_ANAP_MIC_BOOST;
  }

	aic23_writeReg(AIC23_REG_ANALOG_PATH,value);
}

void aic23_shutdown(){
  aic23_setOutputVolume(0,AIC23_CHANNEL_BOTH);
  aic23_setInputVolume(0,AIC23_CHANNEL_BOTH);
  aic23_enableOutput(false);
  aic23_enableInput(false);
  aic23_writeReg(AIC23_REG_POWER_DOWN,AIC23_REG_PWRDN_ALL);
}

void aic23_init(){
  int i;

  //will contain the right values at the end of init
  for(i=0;i<AIC23_NUM_REGS;++i){
    aic23_regValues[i]=0;
  }

  aic23_writeReg(AIC23_REG_RESET,AIC23_REG_RST_RESET);

  //power up the core and setup the digital interface
  aic23_writeReg(AIC23_REG_POWER_DOWN,AIC23_REG_PWRDN_OUTPUT|
	  AIC23_REG_PWRDN_ADC|AIC23_REG_PWRDN_DAC|AIC23_REG_PWRDN_MIC|AIC23_REG_PWRDN_LINEIN);
  aic23_writeReg(AIC23_REG_DIGITAL_INTERFACE_FORMAT,AIC23_REG_DIGFMT_MASTER|
    AIC23_REG_DIGFMT_INPUT_16BIT|AIC23_REG_DIGFMT_FORMAT_DSP);
  aic23_writeReg(AIC23_REG_DIGITAL_INTERFACE_ENABLE,AIC23_REG_DIGINT_ENABLE);

  //everything will be enabled when needed
  aic23_enableOutput(false);
  aic23_enableInput(false);
  aic23_configureInput(AIC23_SOURCE_LINEIN,false);
  aic23_setSampleRate(44100);
  aic23_setOutputVolume(AIC23_NOM_OUTPUT_VOLUME,AIC23_CHANNEL_BOTH);
  aic23_setInputVolume(AIC23_NOM_INPUT_VOLUME,AIC23_CHANNEL_BOTH);

  printk("[init] aic23 codec\n");
}


