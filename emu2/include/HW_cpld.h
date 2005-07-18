/* 
*   HW_cpld.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_CPLD_H
#define __HW_CPLD_H

#include <stdlib.h>
#include <stdio.h>

#include "emu.h"
#include "HW_access.h"
#include "HW_30a24.h"
#include "HW_dma.h"

#define IDE_BASE                          (0x02400000)

#define IDE_DATA                          (IDE_BASE+0x000)
#define IDE_ERROR                         (IDE_BASE+0x080)
#define IDE_NSECTOR                       (IDE_BASE+0x100)
#define IDE_SECTOR                        (IDE_BASE+0x180)
#define IDE_LCYL                          (IDE_BASE+0x200)
#define IDE_HCYL                          (IDE_BASE+0x280)
#define IDE_SELECT                        (IDE_BASE+0x300)
#define IDE_CONTROL                       (IDE_BASE+0x340)
#define IDE_STATUS                        (IDE_BASE+0x380)
#define IDE_COMMAND                       (IDE_BASE+0x380)

#define IDE_CMD_READ_SECTORS              0x20
#define IDE_CMD_WRITE_SECTORS             0x30
#define IDE_CMD_IDENTIFY                  0xec
#define IDE_CMD_SLEEP                     0xe0
#define IDE_CMD_MULT_READ_SECTORS         0xC4
#define IDE_CMD_MULT_WRITE_SECTORS        0xC5

#define IDE_STATUS_BSY                    0x80
#define IDE_STATUS_RDY                    0x40
#define IDE_STATUS_DF                     0x20
#define IDE_STATUS_DRQ                    0x08
#define IDE_STATUS_ERR                    0x01

#define IDE_SEL_LBA                       0x40

#define HAS_ATA

class HW_cpld:public HW_access {
    public:
        HW_cpld();
        ~HW_cpld();  
        
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size); 
        
        void set30A24(HW_30a24 * hw_30a24);
        void setDMA(HW_dma * hw_dma);
        
        void setStatus(int status);
        
        void write_buffer(char * data,int data_size);
        
        int do_cmd_btn(int btn);
        int do_cmd_btn2(void);
        
    private:
        HW_30a24 * hw_30a24;  
        
        HW_dma * hw_dma; 
        
        int cpld_ata_mode;
        int cpld_module_type;
        
        int cpld2_val; 
        
        int fw_connected; 
        int hdd_on;
        int bck_light;
        int ide_reset;
        
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
        
        int btn_var[8];
};

#endif
