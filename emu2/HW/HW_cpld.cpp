/* 
*   HW_cpld.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <HW_cpld.h>

#ifdef HAS_HW_30A24
#include <HW_30a24.h>
#endif

#include <HW_dma.h>
#include <HW_ON_OFF.h>

#include <my_print.h>

#define CPLD_V_LOW
#define CPLD_V_HIGH

#include <hd_data/id_disk.h>
#include <hd_data/part_table.h>

#if 1
#define BTN_FCT(BTN,MASK)      \
    if(BTN)                    \
        BTN--;                 \
    else                       \
        ret_val |= MASK;        
#else
#define BTN_FCT(BTN,MASK)      \
    if(!BTN)                    \
        ret_val |= MASK;        
#endif
HW_cpld * cpld_obj;

#include <cmd_line.h>        
#include "cpld_cmd_line_fct.h"        
        
HW_cpld::HW_cpld(void):HW_access(CPLD_START,CPLD_END,"CPLD")
{
    exit_on_not_match = false;
    
    init_cpld_static_fct(this);
    
    cpld_ata_mode = 0x0 ; /* only HD/CF is concidered => we use HD */
    cpld_module_type = 0xF; /* no modules */
    cpld2_val = 0;
    fw_connected = 1;
    hdd_on =0;
    bck_light = 0;
    ide_reset = 0;
    
    init_ata();
    
    /* btn init */
    for(int k=0;k<8;k++)
        btn_var[k]=0   ;
        
    ON_btn = new HW_ON_OFF(ON_GPIO); 
    OFF_btn = new HW_ON_OFF(OFF_GPIO);
}

HW_cpld::~HW_cpld()
{
    
}

#ifdef HAS_HW_30A24
void HW_cpld::set30A24(HW_30a24 * hw_30a24)
{
    this->hw_30a24 = hw_30a24;
}
#endif

void HW_cpld::setDMA(HW_dma * hw_dma)
{
    this->hw_dma = hw_dma;
}

void HW_cpld::setONOFF(HW_gpio * gpio)
{
    gpio->register_port(ON_btn->gpio_num,ON_btn);
    gpio->register_port(OFF_btn->gpio_num,OFF_btn);
}

uint32_t HW_cpld::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
    
    if(addr >= IDE_BASE && addr < IDE_END)
    {        
        ret_val=ata_read(addr,size);        
    }
    else
    {
        switch(addr)
        {
            case CPLD_START+CPLD_PORT_OFFSET+0x0:
                ret_val=cpld_module_type; 
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD0 - read, size %x: module connected : %x\n",size,ret_val);
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x100:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD1 - read, size %x: %x (USB %s)\n",size,cpld2_val,cpld2_val&0x1?"enable":"disable");
                ret_val = cpld2_val;
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x200:
                ret_val = 0x10;
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD2 - read, size %x: %x\n",size,ret_val);
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x300:
                ret_val = (fw_connected&0x1)<<3;
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD3 - read, size %x: %x\n",size,ret_val);
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x680:
                BTN_FCT(btn_var[0],0x1)
                BTN_FCT(btn_var[1],0x2)
                ret_val |= 0x4;
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x700:
                BTN_FCT(btn_var[2],0x1)
                BTN_FCT(btn_var[3],0x2)
                BTN_FCT(btn_var[4],0x4)
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x780:
                BTN_FCT(btn_var[5],0x1)
                BTN_FCT(btn_var[6],0x2)
                BTN_FCT(btn_var[7],0x4)
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
                break;
            default:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read ERROR ukn addr: @0x%08x, size %x\n",addr,size);
                ret_val=0;
                break;
        }
    }
    return ret_val;
}

void HW_cpld::write(uint32_t addr,uint32_t val,int size)
{
    if(addr >= IDE_BASE && addr < IDE_END)
    {
        ata_write(addr,val,size);
    }
    else
    {
        switch(addr)
        {
            case CPLD_START+CPLD_PORT_OFFSET+0x0:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD0 - write ata mode, size %x: %x\n",size,val);
                cpld_ata_mode = val;
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x100:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD1 - write, size %x: %x (USB %s)\n",size,val,val&0x1?"enable":"disable");
                cpld2_val = val;
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x200:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD2 - write, size %x: %x (bck light %s) (ide reset %s)\n",size,val,
                    (val&0x4)?"ON":"OFF",(val&0x1)?"ON":"OFF");
                bck_light = val&0x4;
                ide_reset = val&0x1;
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x300:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD3 - write, size %x: %x (HD is %s)\n",size,val,(val&0x8)?"ON":"OFF");
                hdd_on = val&0x8;
                hw_30a24->HDD_power = hdd_on?1:0;
                break;
            case CPLD_START+CPLD_PORT_OFFSET+0x680:
            case CPLD_START+CPLD_PORT_OFFSET+0x700:
            case CPLD_START+CPLD_PORT_OFFSET+0x780:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - !!!! write buttons (@0x%08x), size %x\n",addr,size);
                break;
            default:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write ERROR ukn addr: @0x%08x, size %x\n",addr,size);
                break;
        }
    }
}

void HW_cpld::init_ata(void)
{
    status = IDE_STATUS_RDY;
    select_reg = 0;
    h_cyl = 0;
    l_cyl = 0;
    sector = 1;
    nsector = 1;  
    ata_cmd = 0;     
    
    ident_data = id_disk_src;
    part_data = part_table_src;
    buffer = NULL;
       
    hd = fopen("HD.bin", "r+b");
    if (!hd)
        printf("Error: opening: %s for HD emu\n","HD.bin");
}

void HW_cpld::setStatus(int status)
{
    this->status=status;
}

uint32_t HW_cpld::ata_read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
#ifdef HAS_ATA
    switch(addr)
    {
        case IDE_CONTROL:
        case IDE_STATUS:
            ret_val = status;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read %s : %x\n",(addr==IDE_CONTROL)?"CONTROL":"STATUS",ret_val );
            break;
        case IDE_SELECT:
            ret_val = select_reg;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read SELECT : %x\n",ret_val );
            break;
        case IDE_HCYL:
            ret_val = h_cyl;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read HIGH CYL : %x\n",ret_val );
            break;
        case IDE_LCYL:
            ret_val = l_cyl;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read LOW CYL : %x\n",ret_val );
            break;
        case IDE_SECTOR:
            ret_val = sector;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read SECTOR : %x\n",ret_val );
            break;
        case IDE_NSECTOR:
            ret_val = nsector;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read NSECTOR : %x\n",ret_val );
            break;
        case IDE_DATA:
            ret_val = data[data_ptr] & 0xff ;
            data_ptr++;
            ret_val |= (data[data_ptr] & 0xff)<<8;
            data_ptr++;
            if (data_ptr == data_size) {
                data_ptr = 0;
                status = IDE_STATUS_RDY;
                delete(buffer);
                buffer=NULL;
            }
            //DEBUG_HW(ATA_HW_DEBUG,"ATA read DATA : %x\n",ret_val );
            break;
        default:
            //DEBUG_HW(ATA_HW_DEBUG,"ATA unsupported addr for read : %x\n",addr );
            break;        
    }
#endif
    return ret_val;
}

void HW_cpld::ata_write(uint32_t addr,uint32_t val,int size)
{
#ifdef HAS_ATA
    switch(addr)
    {
        case IDE_COMMAND:            
            DEBUG_HW(ATA_HW_DEBUG,"ATA write CMD : %x => ",val );
            ata_cmd = val;
            status = IDE_STATUS_DRQ;
            switch(ata_cmd)
            {
                case IDE_CMD_READ_SECTORS:
                case IDE_CMD_MULT_READ_SECTORS:
                    lba = sector | (l_cyl << 8) | (h_cyl << 16);                    
                    DEBUG_HW(ATA_HW_DEBUG,"read sectors LBA:%x count=%x\n",lba,nsector==0?256:nsector);
                    if(lba == 0 || lba == 1)
                    {
                        data_ptr = 0;
                        data_size = 512;
                        data = part_data+lba*512;
                        //print_data(ATA_HW_DEBUG,data,data_size);
                        hw_dma->init_ata_xfer(data,data_ptr,data_size);
                        break;
                    }
                                        
                    lba -= 0x3F;
                    if(lba>=0)
                    {
                        data_ptr = 0;
                        data_size = 512*(nsector==0?256:nsector);
                        buffer = new char[data_size];
                        data=buffer;
                        if (!hd)
                        {
                            printf("Error: HD file not opened");
                            exit(0);
                        }
                        fseek(hd,lba*512,SEEK_SET);
                        fread(buffer,1,data_size,hd);
                        //print_data(ATA_HW_DEBUG,data,data_size);
                        hw_dma->init_ata_xfer(data,data_ptr,data_size);
                    }     
                    else
                    {
                        data_ptr = 0;
                        data_size = 512*(nsector==0?256:nsector);
                        buffer = new char[data_size];
                        memset(buffer,0,data_size);
                        data = buffer;
                        //print_data(ATA_HW_DEBUG,data,data_size);
                        hw_dma->init_ata_xfer(data,data_ptr,data_size);
                    }               
                    break;
                case IDE_CMD_WRITE_SECTORS:
                case IDE_CMD_MULT_WRITE_SECTORS:
                    lba = sector | (l_cyl << 8) | (h_cyl << 16);
                    DEBUG_HW(ATA_HW_DEBUG,"write sectors LBA:%x\n",lba);
                    lba -= 0x3F;
                    if(lba>=0)
                    {
                        data_ptr = 0;
                        
                        data_size = 512*nsector;
                        buffer = new char[data_size];
                        data=buffer;
                        hw_dma->init_ata_xfer(data,data_ptr,data_size);
                    }                    
                    break;
                case IDE_CMD_IDENTIFY:
                    DEBUG_HW(ATA_HW_DEBUG,"identify\n");
                    data = ident_data;
                    data_ptr = 0;
                    data_size = 512;
                    print_data(ATA_HW_DEBUG,data,data_size);
                    hw_dma->init_ata_xfer(data,data_ptr,data_size);
                    break;
                case IDE_CMD_SLEEP:
                    DEBUG_HW(ATA_HW_DEBUG,"sleep\n");
                    break;
                case IDE_CMD_SET_MULT:
                    DEBUG_HW(ATA_HW_DEBUG,"Set mult XFER\n");
                    break;
                default:
                    printf("UKN cmde %x\n",ata_cmd);
                    break;
            }
            break;
        case IDE_SELECT:
            select_reg = val;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA write SELECT : %x\n",val );
            break;
        case IDE_HCYL:
            h_cyl = val;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA write HIGH CYL : %x\n",val );
            break;
        case IDE_LCYL:
            l_cyl = val;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA write LOW CYL : %x\n",val );
            break;
        case IDE_SECTOR:
            sector = val;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA write SECTOR : %x\n",val );
            break;
        case IDE_NSECTOR:
            nsector = val;
            //DEBUG_HW(ATA_HW_DEBUG,"ATA write SECTOR : %x\n",val );
            break;
        case IDE_DATA:
            data[data_ptr] = val & 0xFF;
            data_ptr++;
            data[data_ptr] = (val>>8) & 0xFF;
            data_ptr++;
            if (data_ptr == data_size) {
                write_buffer(data,data_size);
                data_ptr = 0;
                status = IDE_STATUS_RDY;
                delete(buffer);
                buffer=NULL;
            }
            DEBUG_HW(ATA_HW_DEBUG,"ATA write DATA : %x\n",val );
            break;
        default:
            //DEBUG_HW(ATA_HW_DEBUG,"ATA unsupported addr for write: %x val=%x\n",addr,val );
            break;        
    }
#endif
}

void HW_cpld::write_buffer(char * data,int data_size)
{
    DEBUG_HW(ATA_HW_DEBUG,"ATA writting lba=%x size = %x\n",lba,data_size);
    print_data(ATA_HW_DEBUG,data,data_size);
    if (!hd)
    {
        printf("Error: HD file not opened");
        exit(0);
    }
    fseek(hd,lba*512,SEEK_SET);
    int cnt=fwrite(data,1,data_size,hd);
    DEBUG_HW(ATA_HW_DEBUG,"wrote : %x (%d)\n",cnt,cnt);
    fflush(hd);
}
