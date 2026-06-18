/*
*   lib/ata.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <string.h>

#include <io.h>
#include <kernel.h>
#include <hardware.h>
#include <cpld.h>
#include <ata.h>
#include <dma.h>
#include <delay.h>
#include <disk.h>

#define USE_DMA

int av_cmd_array[]= {
    IDE_CMD_READ_SECTORS,
    IDE_CMD_WRITE_SECTORS
};

#define CALC_BASE(ADDR)     (((unsigned int)(ADDR))-SDRAM_START)

#define SELECT_DRIVE(DRIVE)   ({ \
    if(DRIVE==HD_DRIVE)         \
    {                           \
        ata_select_HD();        \
        ata_powerUp_HD();       \
    }                           \
    else                        \
    {                           \
        ata_select_CF();        \
    }                           \
    })

int ata_process_cmd(ata_cmd_s * ata_cmd)
{
    int i,j;

    if (ata_cmd->xfer_dir == ATA_SLEEP)
    {
        printf ("calling ata_stop_HD...\n");
        ata_stop_HD ();
        return 0;
    }

    /*select the right drive*/

   /* printf("[ata_process_cmd]: mode:%d lba=0x%x count=%d buffer=%08x\n",
        ata_cmd->xfer_dir,ata_cmd->lba,ata_cmd->count,ata_cmd->data);*/

    SELECT_DRIVE(ata_cmd->drive);

    if(ata_waitForReady()<0)
        return -1;

    /* send read/write cmd */
    switch(ata_cmd->xfer_dir)
    {
        case ATA_DO_IDENT:
            outb(0,IDE_SELECT); /* send ident. cmd */
            outb(IDE_CMD_IDENTIFY,IDE_COMMAND);
            ata_cmd->count=1;
            ata_cmd->xfer_dir=ATA_DO_READ;
            ata_cmd->use_dma=ATA_WITH_DMA;
            break;

        case ATA_DO_READ:
        case ATA_DO_WRITE:
            outb(ata_cmd->lba,IDE_SECTOR);
            outb(ata_cmd->lba>>8,IDE_LCYL);
            outb(ata_cmd->lba>>16,IDE_HCYL);
            outb((ata_cmd->lba>>24) | IDE_SEL_LBA,IDE_SELECT);
            outb(ata_cmd->count,IDE_NSECTOR);
            outb(av_cmd_array[ata_cmd->xfer_dir],IDE_COMMAND);
            break;
    }

    //outb(av_cmd_array[ata_cmd->xfer_dir],IDE_COMMAND);

    if(((unsigned int)(ata_cmd->data) < SDRAM_START) && ata_cmd->use_dma==ATA_WITH_DMA)
    {
        printf("Destination buffer not in SDRAM => no DMA\n");
        ata_cmd->use_dma=ATA_NO_DMA;
    }
#warning need to have a config for DMA
    ata_cmd->use_dma=ATA_NO_DMA;

    for(i=0;i<ata_cmd->count;i++)
    {
        if(ata_waitForXfer()<0)
            return 0;

        if(ata_cmd->use_dma==ATA_WITH_DMA)
        {
            if( DMA_RUNNING )
            {
                printf("Error dma is still running\n");
                return -1;
            }

            if((unsigned int)(ata_cmd->data) < SDRAM_START)
            {
                printf("Error buffer not in SDRAM\n");
                return -2;
            }

            if(ata_cmd->xfer_dir==ATA_DO_READ)
            {
                DMA_SET_SRC(DMA_ATA_ADDRESS);
                DMA_SET_DEST((CALC_BASE(ata_cmd->data))+i*SECTOR_SIZE);
                DMA_SET_SIZE(SECTOR_SIZE);
                DMA_SET_DEV(DMA_ATA,DMA_SDRAM)
            }
            else
            {
                DMA_SET_SRC((CALC_BASE(ata_cmd->data))+i*SECTOR_SIZE);
                DMA_SET_DEST(DMA_ATA_ADDRESS);
                DMA_SET_SIZE(SECTOR_SIZE);
                DMA_SET_DEV(DMA_SDRAM,DMA_ATA)
            }

            DMA_START_TRANSFER;

            while(DMA_RUNNING) /*nothing*/;
        }
        else
        {
            //printf("No DMA\n");
            if(ata_cmd->xfer_dir==ATA_DO_READ)
                for(j=0;j<SECTOR_SIZE;j+=2)
                {
                    outw(inw(IDE_DATA),ata_cmd->data+j+i*SECTOR_SIZE);
                }
            else
                for(j=0;j<SECTOR_SIZE;j+=2)
                {
                    outw(inw(ata_cmd->data+j+i*SECTOR_SIZE),IDE_DATA);
                }
        }
    }

    return 0;
}
#if 0
void ata_RW_Data(void * buffer,int count,int direction,int use_dma)
{
    int i;
    unsigned int base;

    /* for testing */
    /*use_dma=ATA_NO_DMA;*/

    if(((unsigned int)(buffer) < SDRAM_START) && use_dma==ATA_WITH_DMA)
    {
        printf("Destination buffer not in SDRAM => no DMA\n");
        use_dma=ATA_NO_DMA;
    }

    if(use_dma==ATA_WITH_DMA)
    {
        if( DMA_RUNNING )
        {
            printf("Error dma is still running\n");
            return;
        }

        if((unsigned int)(buffer) < SDRAM_START)
        {
            printf("Error dma is still running\n");
            return;
        }

        base=((unsigned int)(buffer))&0x00FFFFFF;
        //printf("dma xfer: base=%08x (buffer=%08x)size=%x\n",base,(unsigned int)buffer,count);
        if(direction==ATA_DO_READ)
        {
            DMA_SET_SRC(DMA_ATA_ADDRESS);
            DMA_SET_DEST(base);
            DMA_SET_SIZE(count);
            DMA_SET_DEV(DMA_ATA,DMA_SDRAM)
        }
        else
        {
            DMA_SET_SRC(base);
            DMA_SET_DEST(DMA_ATA_ADDRESS);
            DMA_SET_SIZE(count);
            DMA_SET_DEV(DMA_SDRAM,DMA_ATA)
        }

        DMA_START_TRANSFER;

        while(DMA_RUNNING) /*nothing*/;
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
#endif

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

    printf("[ide sleep]\n");
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

#if 0
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
#else
int ata_identify(int drive,char * buffer)
{
    ata_cmd_s cmd;
    int res;

    cmd.drive=drive;

    cmd.data=buffer;
    cmd.xfer_dir=ATA_DO_IDENT;
    res=ata_process_cmd(&cmd);

    return res;
}
#endif

void ata_reset_HD(void)
{
  arch_ata_reset_HD();
}

void ata_powerUp_HD(void)
{
    arch_ata_powerUp_HD();
}

void ata_powerDown_HD(void)
{
    arch_ata_powerDown_HD();
}

void ata_select_HD(void)
{
    arch_ata_select_HD();
}

void ata_select_CF(void)
{
    arch_ata_select_CF();
}

void init_ata(void)
{
    arch_init_ata();
}

void ide_intr_action(int irq)
{
    arch_ide_intr_action(irq);
}


