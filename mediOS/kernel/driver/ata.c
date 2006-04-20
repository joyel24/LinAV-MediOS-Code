/*
*   kernel/fs/ata.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <kernel/malloc.h>
#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/timer.h>
#include <kernel/hardware.h>
#include <kernel/bat_power.h>
#include <kernel/cpld.h>
//#include <kernel/gio.h>
#include <kernel/ata.h>
#include <kernel/dma.h>
#include <kernel/delay.h>
#include <kernel/disk.h>

#include <kernel/irqs.h>
#include <kernel/irq.h>

//#define USE_DMA

int av_cmd_array[]= {
    IDE_CMD_READ_SECTORS,
    IDE_CMD_WRITE_SECTORS
};

extern int hd_sleep_state;

extern struct tmr_s hd_timer;

#define CALC_BASE(ADDR)     (((unsigned int)(ADDR))-SDRAM_START)

#define ATA_SELECT_DRIVE(DRIVE)   ({ \
    if(DRIVE==HD_DRIVE)         \
    {                           \
        ata_stopping = 0;       \
        ata_selectHD();         \
        ata_powerUpHD();        \
        hd_sleep_state=0;       \
        hd_launchTimer();       \
    }                           \
    else                        \
    {                           \
        ata_selectCF();         \
    }                           \
    })

int ata_rwData(int drive,unsigned int lba,void * data,int count,int cmd,int use_dma)
{
    int i,j;

    /* select the right drive */    
    ATA_SELECT_DRIVE(drive);

    /* wait drive ready */
    if(ata_waitForReady()<0)
        return -1;

    /* send read/write cmd */
    switch(cmd)
    {
        case ATA_DO_IDENT:
            outb(0,IDE_SELECT); /* send ident. cmd */
            outb(IDE_CMD_IDENTIFY,IDE_COMMAND);
            /*ata_cmd->count=1;
            ata_cmd->xfer_dir=ATA_DO_READ;
            ata_cmd->use_dma=ATA_WITH_DMA;*/
            break;

        case ATA_DO_READ:
        case ATA_DO_WRITE:
            outb(lba,IDE_SECTOR);
            outb(lba>>8,IDE_LCYL);
            outb(lba>>16,IDE_HCYL);
            outb((lba>>24) | IDE_SEL_LBA,IDE_SELECT);
            outb(count,IDE_NSECTOR);
            outb(av_cmd_array[cmd],IDE_COMMAND);
            break;
    }

    //outb(av_cmd_array[ata_cmd->xfer_dir],IDE_COMMAND);

    if(((unsigned int)(data) < SDRAM_START) && use_dma==ATA_WITH_DMA)
    {
        printk("Destination buffer not in SDRAM => no DMA\n");    
        use_dma=ATA_NO_DMA;
    }

    for(i=0;i<count;i++)
    {
        if(ata_waitForXfer()<0)
            return 0;

        if(use_dma==ATA_WITH_DMA)
        {
            if( DMA_RUNNING )
            {
                printk("Error dma is still running\n");    
                return -1;
            }

            if((unsigned int)(data) < SDRAM_START)
            {
                printk("Error buffer not in SDRAM\n");    
                return -2;
            }
           
            if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT)
            {
                DMA_SET_SRC(DMA_ATA_ADDRESS);
                DMA_SET_DEST((CALC_BASE(data))+i*SECTOR_SIZE);
                DMA_SET_SIZE(SECTOR_SIZE);
                DMA_SET_DEV(DMA_ATA,DMA_SDRAM)
            }
            else
            {
                DMA_SET_SRC((CALC_BASE(data))+i*SECTOR_SIZE);
                DMA_SET_DEST(DMA_ATA_ADDRESS);
                DMA_SET_SIZE(SECTOR_SIZE);
                DMA_SET_DEV(DMA_SDRAM,DMA_ATA)
            }

            DMA_START_TRANSFER;

            while(DMA_RUNNING) /*nothing*/;
        }
        else
        {
            if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT)
            {
                for(j=0;j<SECTOR_SIZE;j+=2)
                {
                    outw(inw(IDE_DATA),data+j+i*SECTOR_SIZE);
                }
            }
            else
            {
                for(j=0;j<SECTOR_SIZE;j+=2)
                {
                    outw(inw(data+j+i*SECTOR_SIZE),IDE_DATA);
                }
            }
        }
    }

    return 0;
}

int ata_sleep(void)
{
    if(ata_waitForReady()<0)
        return -1;
    outb(0,IDE_SELECT);
    outb(IDE_CMD_SLEEP,IDE_COMMAND);
    return 0;
}

int ata_stopping = 0;

void ata_stopHD(int mode)
{
    printk("[ide sleep] beg\n");
    ata_selectHD();
    ata_sleep();
    if(mode == ATA_FORCE_STOP)
    {
        int i,status;
        ata_stopping = 0;
        for(i=0;i<100;i++)
        {
            mdelay(50);
            status=ata_status();
            if((status & IDE_STATUS_BSY)==0 && (status & IDE_STATUS_RDY)!=0)
                break;
        }
        ata_stopHDEnd();
    }
    else
    {
        ata_stopping = 1;
    }
    
}

void ata_stopHDEnd(void)
{
    ata_powerDownHD();
    tmr_stop(&hd_timer);
    hd_sleep_state=1;
    ata_stopping = 0;
    printk("[ide sleep] end\n");    
}

void ata_sofReset(void)
{
    outb(0xa,IDE_CONTROL);
}

int ata_waitForXfer(void)
{
    int i,val;
    for(i=0;i<WAIT_XFER_TIMEOUT;i++)
    {
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

void ata_resetHD(void)
{
  arch_ata_resetHD();
}

void ata_powerUpHD(void)
{
    arch_ata_powerUpHD();
}

void ata_powerDownHD(void)
{
    arch_ata_powerDownHD();
}

void ata_selectHD(void)
{
    arch_ata_selectHD();
}

void ata_selectCF(void)
{
    arch_ata_selectCF();
}

void ata_init(void)
{
    ata_stopping = 0;
    arch_ata_init();
}

void ide_intAction(int irq,struct pt_regs * regs)
{
    arch_ide_intAction(irq,regs);
}

