/*
*   include/kernel/aic23.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef AIC23_H
#define AIC23_H

#define AIC23_I2C_ADDRESS                   0x36

#define AIC23_CHANNEL_LEFT                  0x01
#define AIC23_CHANNEL_RIGHT                 0x02
#define AIC23_CHANNEL_BOTH                  0x03

#define AIC23_SOURCE_LINEIN                 0x00
#define AIC23_SOURCE_MIC                    0x01

#define AIC23_MAX_INPUT_VOLUME              0x1f
#define AIC23_NOM_INPUT_VOLUME              0x17

#define AIC23_MAX_OUTPUT_VOLUME             0x7f
#define AIC23_NOM_OUTPUT_VOLUME             0x79

#define AIC23_NUM_REGS                      11

#define AIC23_REG_LEFT_INPUT_VOLUME         0x00
#define AIC23_REG_RIGHT_INPUT_VOLUME        0x01
#define AIC23_REG_LEFT_OUTPUT_VOLUME        0x02
#define AIC23_REG_RIGHT_OUTPUT_VOLUME       0x03
#define AIC23_REG_ANALOG_PATH               0x04
#define AIC23_REG_DIGITAL_PATH              0x05
#define AIC23_REG_POWER_DOWN                0x06
#define AIC23_REG_DIGITAL_INTERFACE_FORMAT  0x07
#define AIC23_REG_SAMPLE_RATE               0x08
#define AIC23_REG_DIGITAL_INTERFACE_ENABLE  0x09
#define AIC23_REG_RESET                     0x0a

#define AIC23_REG_OUTVOL_SIMULTANEOUS       0x100
#define AIC23_REG_OUTVOL_ZERO_CROSS_DETECT  0x80

#define AIC23_REG_INVOL_SIMULTANEOUS        0x100
#define AIC23_REG_INVOL_MUTE                0x80

#define AIC23_REG_ANAP_SIDETONE_0DB         0x120
#define AIC23_REG_ANAP_SIDETONE_M6DB        0x020
#define AIC23_REG_ANAP_SIDETONE_M9DB        0x080
#define AIC23_REG_ANAP_SIDETONE_M12DB       0x0a0
#define AIC23_REG_ANAP_SIDETONE_M18DB       0x0e0
#define AIC23_REG_ANAP_DAC_ENABLE           0x10
#define AIC23_REG_ANAP_BYPASS               0x08
#define AIC23_REG_ANAP_INPUT_USE_LINEIN     0x00
#define AIC23_REG_ANAP_INPUT_USE_MIC        0x04
#define AIC23_REG_ANAP_MIC_MUTE             0x02
#define AIC23_REG_ANAP_MIC_BOOST            0x01

#define AIC23_REG_DIGP_DAC_SOFT_MUTE        0x08
#define AIC23_REG_DIGP_DEEMPHASIS_32KHZ     0x02
#define AIC23_REG_DIGP_DEEMPHASIS_44KHZ     0x04
#define AIC23_REG_DIGP_DEEMPHASIS_48KHZ     0x06
#define AIC23_REG_DIGP_ADC_HIGH_PASS        0x01

#define AIC23_REG_PWRDN_NONE                0x00
#define AIC23_REG_PWRDN_ALL                 0xff
#define AIC23_REG_PWRDN_DEVICE              0x80
#define AIC23_REG_PWRDN_CLOCK               0x40
#define AIC23_REG_PWRDN_OSCILLATORS         0x20
#define AIC23_REG_PWRDN_OUTPUT              0x10
#define AIC23_REG_PWRDN_DAC                 0x08
#define AIC23_REG_PWRDN_ADC                 0x04
#define AIC23_REG_PWRDN_MIC                 0x02
#define AIC23_REG_PWRDN_LINEIN              0x01

#define AIC23_REG_DIGFMT_SLAVE              0x00
#define AIC23_REG_DIGFMT_MASTER             0x40
#define AIC23_REG_DIGFMT_DAC_LR_SWAP        0x20
#define AIC23_REG_DIGFMT_DAC_PHASE_0        0x00
#define AIC23_REG_DIGFMT_DAC_PHASE_1        0x10
#define AIC23_REG_DIGFMT_INPUT_16BIT        0x00
#define AIC23_REG_DIGFMT_INPUT_20BIT        0x04
#define AIC23_REG_DIGFMT_INPUT_24BIT        0x08
#define AIC23_REG_DIGFMT_INPUT_32BIT        0x0c
#define AIC23_REG_DIGFMT_FORMAT_DSP         0x03
#define AIC23_REG_DIGFMT_FORMAT_I2S         0x02
#define AIC23_REG_DIGFMT_FORMAT_MSB_FIRST_LEFT_ALIGN  0x01
#define AIC23_REG_DIGFMT_FORMAT_MSB_FIRST_RIGHT_ALIGN 0x00

#define AIC23_REG_SRATE_HALF_INPUT_CLOCK    0x80
#define AIC23_REG_SRATE_HALF_OUTPUT_CLOCK   0x40
#define AIC23_REG_SRATE_OVERSAMPLING_0      0x00
#define AIC23_REG_SRATE_OVERSAMPLING_1      0x02
#define AIC23_REG_SRATE_SAMPLE_RATE_0       0x00
#define AIC23_REG_SRATE_SAMPLE_RATE_1       0x04
#define AIC23_REG_SRATE_SAMPLE_RATE_2       0x08
#define AIC23_REG_SRATE_SAMPLE_RATE_3       0x0c
#define AIC23_REG_SRATE_SAMPLE_RATE_4       0x10
#define AIC23_REG_SRATE_SAMPLE_RATE_5       0x14
#define AIC23_REG_SRATE_SAMPLE_RATE_6       0x18
#define AIC23_REG_SRATE_SAMPLE_RATE_7       0x1c
#define AIC23_REG_SRATE_SAMPLE_RATE_8       0x20
#define AIC23_REG_SRATE_SAMPLE_RATE_9       0x24
#define AIC23_REG_SRATE_SAMPLE_RATE_10      0x28
#define AIC23_REG_SRATE_SAMPLE_RATE_11      0x2c
#define AIC23_REG_SRATE_SAMPLE_RATE_12      0x30
#define AIC23_REG_SRATE_SAMPLE_RATE_13      0x34
#define AIC23_REG_SRATE_SAMPLE_RATE_14      0x38
#define AIC23_REG_SRATE_SAMPLE_RATE_15      0x3c
#define AIC23_REG_SRATE_CLOCK_MODE_NORMAL   0x00
#define AIC23_REG_SRATE_CLOCK_MODE_USB      0x01

#define AIC23_REG_DIGINT_ENABLE             0x01

#define AIC23_REG_RST_RESET                 0x00

int  aic23_readReg(int address);
void aic23_writeReg(int address,int value);
bool aic23_setSampleRate(int rate);
void aic23_setOutputVolume(int volume,int channel);
void aic23_setInputVolume(int volume,int channel);
void aic23_enableOutput(bool enable);
void aic23_enableInput(bool enable);
void aic23_configureInput(int source,bool micboost);
void aic23_shutdown();
void aic23_init();

#endif //AIC23_H

