/*

 All files in this archive are subject to the GNU General Public License.
 See the file COPYING in the source tree root for full license agreement.
 This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 KIND, either express of implied.

 AIC23 audio codec access

 Date:     01/11/2005
 Author:   GliGli

*/

#include <kernel/i2c.h>

#define AIC23_I2C_ADDRESS 0x36

#define AIC23_CHANNEL_BOTH 0
#define AIC23_CHANNEL_LEFT 1
#define AIC23_CHANNEL_RIGHT 2


void aic23WriteReg(int address,int value);
void aic23Init();
void aic23Shutdown();
void aic23SetOutputVolume(int volume,int channel);
