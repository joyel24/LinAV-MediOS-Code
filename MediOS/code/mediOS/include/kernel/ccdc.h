/*
*   include/kernel/ccdc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#ifndef __CCDC_H
#define __CCDC_H

#if defined(DSC25) || defined(DM270) || defined(DM320)
    #define CCDC_BASE                  0x00030700
#elif defined(DSC21)
    #define CCDC_BASE                  0x00030780
#else
    #error NO CHIP defined
#endif

#define CCDC_ENABLE                    (CCDC_BASE + 0x0000)
#define CCDC_SETUP                     (CCDC_BASE + 0x0002)
#define CCDC_VDWIDTH                   (CCDC_BASE + 0x0004)
#define CCDC_HDWIDTH                   (CCDC_BASE + 0x0006)

#define CCDC_ENB_SYNC_ENABLED            0x0001
#define CCDC_ENB_SYNC_DISABLED           0x0000
#define CCDC_ENB_WRITE_ENABLED           0x0002
#define CCDC_ENB_WRITE_DISBLED           0x0000

#define CCDC_STP_SYNC_INPUT             0x0000
#define CCDC_STP_SYNC_OUTPUT            0x0001
#define CCDC_STP_FIELDIND_INPUT         0x0000
#define CCDC_STP_FIELDIND_OUTPUT        0x0002
#define CCDC_STP_VDSYNC_POSITIVE        0x0000
#define CCDC_STP_VDSYNC_NEGATIVE        0x0004
#define CCDC_STP_HDSYNC_POSITIVE        0x0000
#define CCDC_STP_HDSYNC_NEGATIVE        0x0008
#define CCDC_STP_FIELDIND_POSITIVE      0x0000
#define CCDC_STP_FIELDIND_NEAGTIVE      0x0010
#define CCDC_STP_WRITEEN_INTERNAL       0x0000
#define CCDC_STP_WRITEEN_EXTERNAL       0x0020
#define CCDC_STP_DATAPOL_NORMAL         0x0000
#define CCDC_STP_DATAPOL_INVERT         0x0040
#define CCDC_STP_PROGRESIVE             0x0000
#define CCDC_STP_INTERLACED             0x0080
#define CCDC_STP_DATAOUT_8BIT           0x0600
#define CCDC_STP_DATAOUT_9BIT           0x0500
#define CCDC_STP_DATAOUT_10BIT          0x0400
#define CCDC_STP_DATAOUT_11BIT          0x0300
#define CCDC_STP_DATAOUT_12BIT          0x0200
#define CCDC_STP_DATAOUT_13BIT          0x0100
#define CCDC_STP_DATAOUT_14BIT          0x0000
#define CCDC_STP_DATAPACK_NORMAL        0x0000
#define CCDC_STP_DATAPACK_PACKED        0x0800
#define CCDC_STP_DATAIN_CCDRAW          0x0000
#define CCDC_STP_DATAIN_YCBCR16BIT      0x1000
#define CCDC_STP_DATAIN_YCBCR8BIT       0x2000
#define CCDC_STP_FILTER_OFF             0x0000
#define CCDC_STP_FILTER_ON              0x4000
#define CCDC_STP_FIELD_ODD              0x0000
#define CCDC_STP_FIELD_EVEN             0x8000

#endif
