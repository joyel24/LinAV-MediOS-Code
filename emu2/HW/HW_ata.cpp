/* 
*   HW_ata.cpp
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

#include <emu.h>
#include <HW_ata.h>

#include <HW_dma.h>

#include <my_print.h>

#define HAS_ATA

#include <hd_data/id_disk.h>
#include <hd_data/part_table.h>


HW_ata::HW_ata(void):HW_access(IDE_BASE,IDE_END,"ATA")
{
    exit_on_not_match = false;
    
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
    printf("ATA init done (%x-%x)\n",this->start,this->end);
}

HW_ata::~HW_ata()
{
    
}

void HW_ata::setDMA(HW_dma * hw_dma)
{
    this->hw_dma = hw_dma;
}

uint32_t HW_ata::read(uint32_t addr,int size)
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
            DEBUG_HW(ATA_HW_DEBUG,"ATA unsupported addr for read : %x\n",addr );
            break;        
    }
#endif
    return ret_val;
}

void HW_ata::write(uint32_t addr,uint32_t val,int size)
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
            DEBUG_HW(ATA_HW_DEBUG,"ATA unsupported addr for write: %x val=%x\n",addr,val );
            break;        
    }
#endif
}

void HW_ata::setStatus(int status)
{
    this->status=status;
}

void HW_ata::write_buffer(char * data,int data_size)
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
