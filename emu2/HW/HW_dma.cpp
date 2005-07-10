/* 
*   HW_dma.cpp
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

#include <HW_dma.h>
#include "HW_cpld.h"


HW_dma::HW_dma(HW_mem * mem,HW_cpld * hw_cpld):HW_access(0x30a38,0x30a48,"DMA")
{
    dma_src=dma_dst=dma_size=0;
    device_sel=dma_endian=0;
    this->mem=mem;
    this->hw_cpld=hw_cpld;
}

uint32_t HW_dma::read(uint32_t addr,int size)
{
    int ret_val = 0;
#ifdef HAS_ATA
    switch(addr)
    {
        case 0x30a38:            
            ret_val = (dma_src >> 16)&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - SRC HI = %x\n",ret_val);
            break;
        case 0x30a3a:            
            ret_val = dma_src&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - SRC LO = %x\n",ret_val);
            break;
        case 0x30a3c:            
            ret_val = (dma_dst >> 16)&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DST HI = %x\n",ret_val);
            break;
        case 0x30a3e:            
            ret_val = dma_src&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DST LO = %x\n",ret_val);
            break;
        case 0x30a40:            
            ret_val = dma_size;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - SIZE = %x\n",ret_val);
            break;
        case 0x30a42:            
            ret_val = device_sel;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DEVICE SELECT = %x\n",ret_val);
            break;
        case 0x30a44:            
            ret_val = 0;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DMA state = %x\n",ret_val);
            break;
        default:
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - BAD address: %x\n",addr);
            break;
    }
#endif
    return ret_val;
}

void HW_dma::write(uint32_t addr,uint32_t val,int size)
{
#ifdef HAS_ATA
    switch(addr)
    {
        case 0x30a38:            
            dma_src = (dma_src&0xFFFF) | ((val << 16)&0xFFFF0000);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - SRC HI = %x (src=%x)\n",val,dma_src);
            break;
        case 0x30a3a:            
            dma_src = (dma_src&0xFFFF0000) | (val & 0xFFFF);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - SRC LO = %x (src=%x)\n",val,dma_src);
            break;
        case 0x30a3c:            
            dma_dst = (dma_dst&0xFFFF) | ((val << 16)&0xFFFF0000);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - DST HI = %x (dst=%x)\n",val,dma_dst);
            break;
        case 0x30a3e:            
            dma_dst = (dma_dst&0xFFFF0000) | (val & 0xFFFF);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - DST LO = %x (dst=%x)\n",val,dma_dst);
            break;
        case 0x30a40:            
            dma_size = val;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - SIZE = %x\n",val);
            break;
        case 0x30a42:            
            device_sel = val&0xFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - DEVICE SELECT = %x\n",val);
            break;
        case 0x30a44:
            dma_endian = (val>>8)&0x1;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - START - (endian=%x)",dma_endian);
            if(val & 0x1)
            {
                DEBUG_HW(DMA_HW_DEBUG," - xfer from %x to %x, size = %x, dir : %s->%s ... ",dma_src,dma_dst,dma_size,
                    ((device_sel>>4)&0xf)==1?"ATA":"SDRAM",(device_sel&0xf)==1?"ATA":"SDRAM");
                switch(device_sel)
                {     
                    case 0x51:                          
                        for (int i = 0; i < dma_size; i++)
                            data[i+data_ptr] = mem->read(0x03000000 + dma_src + i,1);
                        data_ptr+=dma_size;
                        dma_src+=dma_size;                        
                        if(data_ptr==data_size)
                        {
                            DEBUG_HW(DMA_HW_DEBUG," (finale) \n");
                            hw_cpld->write_buffer(data,data_size);
                            hw_cpld->setStatus(IDE_STATUS_RDY);
                        }                        
                        DEBUG_HW(DMA_HW_DEBUG,"done");
                        break;
                    case 0x15:
                        DEBUG_HW(DMA_HW_DEBUG,"real dest = %x , src val (%x) %02x%02x%02x%02x  ",0x03000000 + dma_dst,data_ptr,
                                data[data_ptr]&0xFF,data[data_ptr+1]&0xFF,data[data_ptr+2]&0xFF,data[data_ptr+3]&0xFF);
                        for (int i = 0; i < dma_size; i++)
                             mem->write(0x03000000 + dma_dst + i,data[i+data_ptr] & 0xff,1);
                        data_ptr+=dma_size;
                        dma_dst+=dma_size;
                        if(data_ptr==data_size)
                        {
                            DEBUG_HW(DMA_HW_DEBUG," (finale) ");
                            hw_cpld->setStatus(IDE_STATUS_RDY);
                        }
                        DEBUG_HW(DMA_HW_DEBUG,"done");
                        break;
                    default:
                        DEBUG_HW(DMA_HW_DEBUG,"error bad dev select: %x",device_sel);
                        break;
                }
            }            
            DEBUG_HW(DMA_HW_DEBUG,"\n");
            break;
        default:
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - BAD address: %x\n",addr);
            break;
    }
#endif
}

void HW_dma::init_ata_xfer(char * data,int data_ptr,int data_size)
{
    this->data = data;
    this->data_ptr = data_ptr;
    this->data_size = data_size;
}
