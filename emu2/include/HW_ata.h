/*
*   HW_ata.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_ata_H
#define __HW_ata_H

#include <stdlib.h>
#include <stdio.h>

#include "emu.h"
#include "HW_access.h"
#include "HW_dma.h"


#define IDE_CMD_READ_SECTORS              0x20
#define IDE_CMD_WRITE_SECTORS             0x30
#define IDE_CMD_IDENTIFY                  0xec
#define IDE_CMD_SLEEP                     0xe0
#define IDE_CMD_MULT_READ_SECTORS         0xC4
#define IDE_CMD_MULT_WRITE_SECTORS        0xC5
#define IDE_CMD_SET_MULT                  0xc6


#define IDE_STATUS_BSY                    0x80
#define IDE_STATUS_RDY                    0x40
#define IDE_STATUS_DF                     0x20
#define IDE_STATUS_DRQ                    0x08
#define IDE_STATUS_ERR                    0x01

#define IDE_SEL_LBA                       0x40

#define HAS_ATA

class HW_ata:public HW_access {
    public:
        HW_ata();
        ~HW_ata();  
        
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size); 
        
        void setStatus(int status);        
        void setDMA(HW_dma * hw_dma);
        
        void write_buffer(char * data,int data_size);
        
        
    private:
        
        HW_dma * hw_dma; 
                
        void init_ata(void);
        uint32_t ata_read(uint32_t addr,int size);
        void ata_write(uint32_t addr,uint32_t val,int size); 
        
        int status,select_reg,h_cyl,l_cyl,sector,nsector,ata_cmd;
        uint32_t lba;
        
        char * data;
        int data_ptr;
        int data_size;
        
        char * ident_data;
        char * part_data;
        char * buffer;
        
        FILE * hd;
        
        
};

#endif
