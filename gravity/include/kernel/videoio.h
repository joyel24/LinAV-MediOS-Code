/* 
*   kernel/videoio.h
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __VIDEOIO_H
#define __VIDEOIO_H

#include <kernel/errors.h>

#define VIDEOIO_SYNCEN_REG   0x00
#define VIDEOIO_MODESET_REG  0x02
#define VIDEOIO_CLAMP_REG    0x04
#define VIDEOIO_HDVDW_REG    0x06
#define VIDEOIO_PPLN_REG     0x08
#define VIDEOIO_LPFR_REG     0x0A
#define VIDEOIO_SPH_REG      0x0C
#define VIDEOIO_LNH_REG      0x0E
#define VIDEOIO_SLV_REG      0x10
#define VIDEOIO_LNV_REG      0x12
#define VIDEOIO_DECIM_REG    0x14
#define VIDEOIO_HSIZE_REG    0x16
#define VIDEOIO_SDA_HI_REG   0x18
#define VIDEOIO_SDA_LO_REG   0x1A
#define VIDEOIO_VDINT0_REG   0x1C
#define VIDEOIO_VDINT1_REG   0x1E
#define VIDEOIO_OSDHPOS_REG  0x20
#define VIDEOIO_OSDVPOS_REG  0x22
#define VIDEOIO_FIDMODE_REG  0x24

#endif //__VIDEOIO_H
