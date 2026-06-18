#include <linux/kernel.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <linux/hdreg.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_module.h>
#include <asm/arch/av3xx_cpld.h>
#include <asm/arch/av3xx_gio.h>
#include <asm/arch/av3xx_ata_ide.h>

#define AV_SECTOR_SIZE      512

int av_cmd_array[]= {
    AV3XX_IDE_CMD_READ_SECTORS,
    AV3XX_IDE_CMD_WRITE_SECTORS
};

int av_ata_RW_Sector(unsigned int lba,int count,void * buffer,int direction)
{
    int i;
    //printk("[RW_Sector] mode:%d lba=0x%x count=%d\n",direction, lba,count);
    
    if(av_ata_waitForReady()<0)
        return -1;
    /* send read/write cmd */
    outb(lba,AV3XX_IDE_SECTOR);
    outb(lba>>8,AV3XX_IDE_LCYL);
    outb(lba>>16,AV3XX_IDE_HCYL);
    outb((lba>>24) | AV3XX_IDE_SEL_LBA,AV3XX_IDE_SELECT);
    outb(count,AV3XX_IDE_NSECTOR);
    outb(av_cmd_array[direction],AV3XX_IDE_COMMAND);
    /* begin read */
    for(i=0;i<count;i++)
    {
        if(av_ata_waitForXfer()<0)
            return i;
        av_ata_RW_Data(buffer+AV_SECTOR_SIZE*i,AV_SECTOR_SIZE,direction);
    }
    return i;
}


void av_ata_RW_Data(void * buffer,int count,int direction)
{
    int i;
    if(direction==AV_DO_READ)
        for(i=0;i<count;i+=2)
        {
            outw(inw(AV3XX_IDE_DATA),buffer+i);
        }
    else
        for(i=0;i<count;i+=2)
        {
            outw(inw(buffer+i),AV3XX_IDE_DATA);
        }
}

int av_ata_sleep(void)
{
    if(av_ata_waitForReady()<0)
        return -1;
    outb(0,AV3XX_IDE_SELECT);
    outb(AV3XX_IDE_CMD_SLEEP,AV3XX_IDE_COMMAND);
    return 0;
}

void av_ata_stop_HD(void)
{
    int j,status;
    av_ata_select_HD();
    av_ata_sleep();
    for(j=0;j<50;j++)
    {
        mdelay(100);
        status=av_ata_status();
        if((status&AV3XX_IDE_STATUS_BSY)==0 && (status&AV3XX_IDE_STATUS_RDY)!=0)
            break;
    }    
    av_ata_powerDown_HD();    
    udelay(100);
    printk("[ide sleep]\n");
}

int av_ata_waitForXfer(void)
{
    int i,val;
    for(i=0;i<AV_WAIT_XFER_TIMEOUT;i++)
    {
        val=inb(AV3XX_IDE_CONTROL);
        if((val&AV3XX_IDE_STATUS_BSY)==0 && (val&AV3XX_IDE_STATUS_DRQ)!=0)
            return 0;
    }
    return -1; /* if we are here => we have a timeout */
}

int av_ata_waitForReady(void)
{
    int i,val;
    for(i=0;i<AV_WAIT_READY_TIMEOUT;i++)
    {
        val=inb(AV3XX_IDE_CONTROL);
        if((val&AV3XX_IDE_STATUS_BSY)==0 && (val&AV3XX_IDE_STATUS_RDY)!=0)
            return 0;
    }
    return -1; /* if we are here => we have a timeout */
}       

int av_ata_status(void)
{
    return inb(AV3XX_IDE_CONTROL);
}
      
void av_ata_powerUp_HD(void)
{
    cpld_set_port_3(AV3XX_CPLD_HD_POWER);
}

void av_ata_powerDown_HD(void)
{
    cpld_clear_port_3(AV3XX_CPLD_HD_POWER);
}        
        
void av_ata_select_HD(void)
{
    cpld_select(AV3XX_CPLD_HD_CF,AV3XX_CPLD_SEL_HD);
}

void av_ata_select_CF(void)
{
    cpld_select(AV3XX_CPLD_HD_CF,AV3XX_CPLD_SEL_CF);
}

int av_ata_readMBR(struct av_partInfo * part_list)
{
    int res;
    char buffer[AV_SECTOR_SIZE];
    int part_num;
    int offset;
    if((res=av_ata_RW_Sector(0,1,buffer,AV_DO_READ))<0) /* read 1 sector at LBA 0 */
        return res;
    for(part_num=0;part_num<4;part_num++)
    {
        offset=446+part_num*16;
        part_list[part_num].type=buffer[offset+4]&0xFF;
        part_list[part_num].start=buffer[offset+8] | (buffer[offset+8+1]<<8) | (buffer[offset+8+2]<<16) | (buffer[offset+8+3]<<24);
        part_list[part_num].size=buffer[offset+12] | (buffer[offset+12+1]<<8) | (buffer[offset+12+2]<<16) | (buffer[offset+12+3]<<24);
    }
    /* some sanity check */
    if(part_list[0].start == 0x00)
        printk("[READ MBR] start of first partion should be >0\n");

    if(buffer[510]!=0x55 || buffer[511]!=0xAA)
            printk("[READ MBR]: mbr bad, missing 55AA at the end of mbr: %02x%02x\n",buffer[510],buffer[511]);
    return 0;
}

void swapChar(char * txt,int size)
{
    int i;
    for(i=0;i<size/2;i++)
    {
            char c=txt[2*i];
            txt[2*i]=txt[2*i+1];
            txt[2*i+1]=c;
    }
}

void ata_findEnd(char * txt,int size)
{
    int i,j;
    /* find first real char */
    for(i=0;i<size;i++)
        if(txt[i]!=' ')
            break;
    /* remove head space */
    for(j=i;j<size;j++)
    {
        if(txt[j]==' ')
        {
            txt[j-i]=0;
            break;
         }
         txt[j-i]=txt[j];    
    }
    if(txt[j-i]!=0)
        txt[j-i]=0;
}
               
int av_ata_identify(struct av_hd_info * hd_info)
{
    int res;
    //char buffer[AV_SECTOR_SIZE];
    struct hd_driveid ident_res;
    void * buffer = (void*)&ident_res;
    
    if(av_ata_waitForReady()<0)
        return -1;
    /* send ident. cmd */
    outb(0,AV3XX_IDE_SELECT);
    outb(AV3XX_IDE_CMD_IDENTIFY,AV3XX_IDE_COMMAND);
    /* begin read */
    if(av_ata_waitForXfer()<0)
        return -2;
    
    av_ata_RW_Data(buffer,AV_SECTOR_SIZE,AV_DO_READ);
    
    strncpy(hd_info->serial, &buffer[20], 20);
    swapChar(hd_info->serial,20);
    ata_findEnd(hd_info->serial,20);
    strncpy(hd_info->firmware, &buffer[46], 8);
    swapChar(hd_info->firmware,8);
    ata_findEnd(hd_info->firmware,8);
    strncpy(hd_info->model, &buffer[54], 40);
    swapChar(hd_info->model,40);
    ata_findEnd(hd_info->model,40);
    
    printk("capa=%d cyl=%d head=%d sect=%d\n",ident_res.lba_capacity,ident_res.cur_cyls,ident_res.cur_heads,ident_res.cur_sectors);
    
    return 0;
}   

