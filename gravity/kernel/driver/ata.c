/* 
*   kernel/fs/ata.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/timer.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
//#include <kernel/gio.h>
#include <kernel/ata.h>
#include <kernel/dma.h>
#include <kernel/delay.h>
//#include <kernel/disk.h>

//#define USE_DMA

int av_cmd_array[]= {
    IDE_CMD_READ_SECTORS,
    IDE_CMD_WRITE_SECTORS
};

extern struct timer_s hd_timer;

int ata_RW_Sector(unsigned int lba,int count,void * buffer,int direction)
{
    int i;
    
    //printk("[RW_Sector] mode:%d lba=0x%x count=%d buffer=%08x\n",direction, lba,count,buffer);
    
    if(ata_waitForReady()<0)
        return -1;
    /* send read/write cmd */
    outb(lba,IDE_SECTOR);
    outb(lba>>8,IDE_LCYL);
    outb(lba>>16,IDE_HCYL);
    outb((lba>>24) | IDE_SEL_LBA,IDE_SELECT);
    outb(count,IDE_NSECTOR);
    outb(av_cmd_array[direction],IDE_COMMAND);
    /* begin read/write */

    for(i=0;i<count;i++)
    {
        if(ata_waitForXfer()<0)
            return 0;
        ata_RW_Data(buffer+SECTOR_SIZE*i,SECTOR_SIZE,direction,ATA_WITH_DMA);
    }
    
    return 0;
}

void ata_RW_Data(void * buffer,int count,int direction,int use_dma)
{
    int i;   
    unsigned int base;
    if(((unsigned int)(buffer) < 0x03000000) && use_dma==ATA_WITH_DMA)
    {
        printk("Destination buffer not in SDRAM => no DMA\n");    
        use_dma=ATA_NO_DMA;
    }
    
    if(use_dma==ATA_WITH_DMA)
    {   
        if( dma_running )
        {
            printk("Error dma is still running\n");    
            return;
        }
        
        if((unsigned int)(buffer) < 0x03000000)
        {
            printk("Error dma is still running\n");    
            return;
        }
        
        base=((unsigned int)(buffer))&0x00FFFFFF;
        //printk("dma xfer: base=%08x (buffer=%08x)size=%x\n",base,(unsigned int)buffer,count);    
        if(direction==ATA_DO_READ)
        {
            dma_set_src(0x10400000);
            dma_set_dest(base);
            dma_set_size(count);
            dma_set_dev(DMA_ATA,DMA_SDRAM)
        }
        else
        {            
            dma_set_src(base);
            dma_set_dest(0x10400000);
            dma_set_size(count);
            dma_set_dev(DMA_SDRAM,DMA_ATA)
        }
        
        dma_start
            
        while(dma_running) /*nothing*/;
    }
    else
    {
        if(direction==ATA_DO_READ)
            for(i=0;i<count;i+=2)
            {
                outw(inw(IDE_DATA),buffer+i);
            }
        else
            for(i=0;i<count;i+=2)
            {
                outw(inw(buffer+i),IDE_DATA);
            }
    }
}

int ata_sleep(void)
{
    if(ata_waitForReady()<0)
        return -1;
    outb(0,IDE_SELECT);
    outb(IDE_CMD_SLEEP,IDE_COMMAND);
    return 0;
}

void ata_stop_HD(void)
{
    int j,status;
    ata_select_HD();
    ata_sleep();
    for(j=0;j<100;j++)
    {
        mdelay(50);
        status=ata_status();
        if((status & IDE_STATUS_BSY)==0 && (status & IDE_STATUS_RDY)!=0)
            break;
    }    
    ata_powerDown_HD();    
    //udelay(100);
    printk("[ide sleep]\n");
    stop_timer(&hd_timer);
}

int ata_waitForXfer(void)
{
    int i,val;
    for(i=0;i<WAIT_XFER_TIMEOUT;i++)
    {
        //udelay(10);
        val=inb(IDE_CONTROL);
        if((val & IDE_STATUS_BSY)==0 && (val & IDE_STATUS_DRQ)!=0)
            return 0;
    }
    return -1; /* if we are here => we have a timeout */
}

int ata_waitForReady(void)
{
    int i,val;
    for(i=0;i<WAIT_READY_TIMEOUT;i++)
    {
        //udelay(10);
        val=inb(IDE_CONTROL);
        if((val & IDE_STATUS_BSY)==0 && (val & IDE_STATUS_RDY)!=0)
            return 0;
    }
    return -1; /* if we are here => we have a timeout */
}       

int ata_status(void)
{
    return inb(IDE_CONTROL);
}
      
void ata_powerUp_HD(void)
{
    cpld_set_port_3(CPLD_HD_POWER);
}

void ata_powerDown_HD(void)
{
    cpld_clear_port_3(CPLD_HD_POWER);
}        
        
void ata_select_HD(void)
{
    cpld_select(CPLD_HD_CF,CPLD_SEL_HD);
}

void ata_select_CF(void)
{
    cpld_select(CPLD_HD_CF,CPLD_SEL_CF);
}
              
int ata_identify(char * buffer)
{
    if(ata_waitForReady()<0)
        return -1;
    /* send ident. cmd */
    outb(0,IDE_SELECT);
    outb(IDE_CMD_IDENTIFY,IDE_COMMAND);
    /* begin read */
    if(ata_waitForXfer()<0)
        return -2;
    
    ata_RW_Data(buffer,SECTOR_SIZE,ATA_DO_READ,ATA_NO_DMA);    
    return 0;
}   
