#include <linux/module.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/hdreg.h>



#define AV_IDE_MAJOR         10
#define HD_FREQ_DEFAULT_0    10 /* 10s timer */
#define HD_FREQ_DEFAULT_1    10 /* 10s timer */

#define MAJOR_NR             AV_IDE_MAJOR
#define AV_IDE_SHIFT         3                      /* 4 part. max */
#define AV_IDE_MAXNRDEV      2                      /* 1 units max */
#define DEVICE_NR(device)    (MINOR(device)>>AV_IDE_SHIFT)
#define DEVICE_NAME          "av_ide"
//#define DEVICE_INTR          
#define DEVICE_NO_RANDOM
#define DEVICE_REQUEST       av_ide_request
/***************************************************************
* FIXME use this to validate the CPLD state
***************************************************************/
#define DEVICE_ON(d)         /*nothing*/
#define DEVICE_OFF(d)        /*nothing*/

#include <linux/blk.h>
#include <linux/blkpg.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_module.h>
#include <asm/arch/av3xx_buttons.h>
#include <asm/arch/av3xx_cpld.h>
#include <asm/arch/av3xx_gio.h>
#include <asm/arch/av3xx_ata_ide.h>
#include <asm/arch/av3xx_ide.h>


int * av_ide_sizes = NULL;
int nbHDPartition=0;
struct av_hd_info hd_info;
struct av_partInfo part_list[4];

int device_busy[1 << AV_IDE_SHIFT];

struct block_device_operations av_ide_bdops;
struct gendisk av_ide_gendisk;
struct hd_struct *av_ide_partitions = NULL;

spinlock_t av3xx_ide_drive_lock[2];
spinlock_t av3xx_ide_bus_lock;

struct timer_list av3xx_hd_timer;

int hd_freq_rep[2]={HD_FREQ_DEFAULT_0,HD_FREQ_DEFAULT_1};
int av3xx_hd_timer_used[2]={1,1};
int av3xx_hd_sleep_state=0;

int getCurrentTimer(void);

int av_ide_open (struct inode *inode, struct file *filp)
{
    return 0;
}

int av_ide_release (struct inode *inode, struct file *filp)
{
    fsync_dev(inode->i_rdev);
    invalidate_buffers(inode->i_rdev);
    return 0;
}

int av_ide_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct hd_geometry * geo=(struct hd_geometry *)arg;
    long * val=(long*)arg;
    
    switch(cmd)
    {
        case BLKGETSIZE:
            *val=(long) av_ide_gendisk.part[MINOR(inode->i_rdev)].nr_sects;
            return 0;
        case BLKFLSBUF: /* flushing buff */
            if (! capable(CAP_SYS_RAWIO)) return -EACCES; /* only root can do it */
            fsync_dev(inode->i_rdev);
            invalidate_buffers(inode->i_rdev);
            return 0;
        case BLKRAGET:
            *val=(long)read_ahead[MAJOR(inode->i_rdev)];
            return 0;
        case BLKRASET:
            read_ahead[MAJOR(inode->i_rdev)] = *val;
            return 0;
        case BLKRRPART:
            return av_ide_revalidate(inode->i_rdev);
        case HDIO_GETGEO:
            geo->cylinders = 10;
            geo->heads = 4;
            geo->sectors = 16;
            geo->start = 4;        
            return 0;
        default:
            return blk_ioctl(inode->i_rdev, cmd, arg);
    }
    return -ENOTTY; /* unknown ioctl */
}

int av_ide_check_change(kdev_t i_rdev)
{
    return 0;
}

int av_ide_revalidate(kdev_t i_rdev)
{    
    return 0;
}

struct block_device_operations av_ide_bdops = {
    open:               av_ide_open,
    release:            av_ide_release,
    ioctl:              av_ide_ioctl,
    check_media_change: av_ide_check_change,
    revalidate:         av_ide_revalidate,
};

struct gendisk av_ide_gendisk = {
    major:              AV_IDE_MAJOR, 
    major_name:         "av_ide",
    minor_shift:        AV_IDE_SHIFT,
    max_p:              1 << AV_IDE_SHIFT,
    fops:               &av_ide_bdops,
    nr_real:            AV_IDE_MAXNRDEV 
};

//#define IDE_MODE_DMA 1

int av_ide_transfer(const struct request * req)
{
     int res;
     int size, minor,start;
     minor = MINOR(req->rq_dev);
     start = av_ide_partitions[minor].start_sect+req->sector;
     size = req->current_nr_sectors;

     /* checking size and start */
     if(req->sector+req->current_nr_sectors > av_ide_partitions[minor].nr_sects)
         return 0;
     
    spin_lock(&av3xx_ide_bus_lock);
    if(DEVICE_NR(req->rq_dev)==0)
    {
        av_ata_select_HD();
        av_ata_powerUp_HD();
        av3xx_hd_sleep_state=0;
        av3xx_hd_launchTimer();
    }
    else
    {
        av_ata_select_CF();        
        //printk("using CF\n");
    }
    
   // printk("%s: lba=0x%x size=%d\n",req->cmd==READ?"Read":"Write",start,size);
         
    switch(req->cmd)
    {
        case READ:
#ifdef IDE_MODE_DMA
            res=av_ata_RW_DMA_Sector(start,size*512,req->buffer,AV_DO_READ);
#else
            res=av_ata_RW_Sector(start,size,req->buffer,AV_DO_READ);
#endif
            if(res<0)
            {
                    res=0;
                    break;
            }
            if(res!=req->current_nr_sectors)
                res=0;
            else
                res=1;
            break;
        case WRITE:
#ifdef IDE_MODE_DMA
            res=av_ata_RW_DMA_Sector(start,size*512,req->buffer,AV_DO_WRITE);
#else
            res=av_ata_RW_Sector(start,size,req->buffer,AV_DO_WRITE);
#endif
            if(res<0)
            {
                    res=0;
                    break;
            }
            if(res!=req->current_nr_sectors)
                return 0;
            else
                res=1;
            break;
        default:
            /* should not happen */
            res=0;
            break;
    }
    spin_lock(&av3xx_ide_bus_lock);
    return res;
}

void av_ide_request(request_queue_t *q)
{
    
    struct request *req=blkdev_entry_next_request(&q->queue_head);
    int status;
    int deviceNum=DEVICE_NR(req->rq_dev);
    
#if 1    
    if (device_busy[deviceNum])
        return;
    device_busy[deviceNum] = 1;

    while(! list_empty(&q->queue_head)) {
    /* Remove the request from the list */
        req = blkdev_entry_next_request(&q->queue_head);
        blkdev_dequeue_request(req);
        spin_unlock_irq (&io_request_lock);
        /* check if needed */
        //spin_lock(&device->lock);

    /* working out all the buffers */
        do {
            status = av_ide_transfer(req);
        } while (end_that_request_first(req, status, DEVICE_NAME));
        /* check if needed */
        //spin_unlock(&device->lock);
        spin_lock_irq (&io_request_lock);
        end_that_request_last(req);
    }
    device_busy[deviceNum] = 0;
#else
    while(1)
    {
        INIT_REQUEST;  /* retour à l'état antérieur lorsque la file d'attente est vide */
        
        status = av_ide_transfer(CURRENT);
        
        end_request(status); /* réussi */
    }
#endif
}

void av3xx_ide_halt(void)
{
    /* we'll need to add some flush here */
    av3xx_hd_sleep_state=1;
    av_ata_stop_HD();
}

int CF_reader_connected=0;

void av3xx_cf_connected(void)
{
    int i;
    int part1 = 1 << AV_IDE_SHIFT;
    int npart = (1 << AV_IDE_SHIFT) -1;
    
    printk("[CF_reader_connected] set\n");
    CF_reader_connected=1;
    
    memset(av_ide_gendisk.sizes+part1, 0, npart*sizeof(int));
    memset(av_ide_gendisk.part +part1, 0, npart*sizeof(struct hd_struct));
    for(i=0;i<0x20000;i++) /*nothing*/;
    register_disk(&av_ide_gendisk,MKDEV(AV_IDE_MAJOR,1<<AV_IDE_SHIFT),4,&av_ide_bdops,-1);
    av3xx_add_event(EVT_AV300_CF_IN);
}

void av3xx_cf_disconnected(void)
{
    int part1 = 1 << AV_IDE_SHIFT;
    int npart = (1 << AV_IDE_SHIFT) -1;

    memset(av_ide_gendisk.sizes+part1, 0, npart*sizeof(int));
    memset(av_ide_gendisk.part +part1, 0, npart*sizeof(struct hd_struct));
    CF_reader_connected=0;
    av3xx_add_event(EVT_AV300_CF_OUT);
}

void av3xx_cf_reader_connected(void)
{
    if(CF_reader_connected)
    {
        av3xx_cf_disconnected();    
    }
    CF_reader_connected=1;
}

void av3xx_cf_reader_disconnected(void)
{
    CF_reader_connected=0;
}

struct module_actions avcf_module_actions = {
    do_connection:av3xx_cf_connected,
    do_disconnection:av3xx_cf_disconnected
};

struct module_actions avcf_reader_module_actions = {
    do_connection:av3xx_cf_reader_connected,
    do_disconnection:av3xx_cf_reader_disconnected
};

void av3xx_hd_launchTimer(void)
{
    int num=getCurrentTimer();
    if(hd_freq_rep[num]!=0 && av3xx_hd_timer_used[num] && !av3xx_hd_sleep_state)
    {
        del_timer(&av3xx_hd_timer);
        av3xx_hd_timer.expires = jiffies + (hd_freq_rep[num]*HZ); /* hd_freq_rep in sec */
        add_timer(&av3xx_hd_timer);
    }
}

void av3xx_hd_timer_fct(unsigned long ptr)
{
    int num=getCurrentTimer();
    if(av3xx_hd_timer_used[num])
        av_ata_stop_HD();
}

void av3xx_hd_timer_on(int num)
{
    av3xx_hd_timer_used[num]=1;
    if(num == getCurrentTimer())
        av3xx_hd_launchTimer();
}

int av3xx_hd_timer_state(int num)
{
    return av3xx_hd_timer_used[num];
}

void av3xx_hd_timer_off(int num)
{
    av3xx_hd_timer_used[num]=0;
    del_timer(&av3xx_hd_timer);
}

int av3xx_ide_init(void)
{
    int res,i;
    
    printk("av3xx-IDE-CF driver by oxygen77@free.fr\n");
    
    /* setting up spinlocks */
    
    spin_lock_init(&av3xx_ide_drive_lock[0]);
    spin_lock_init(&av3xx_ide_drive_lock[1]);
    spin_lock_init(&av3xx_ide_bus_lock);
    
    for(i=0;i<(1 << AV_IDE_SHIFT);i++)
        device_busy[i]=0;
        
    /* setting up a timer to stop the HD */
    init_timer(&av3xx_hd_timer);
    av3xx_hd_timer.function = av3xx_hd_timer_fct;
        
    /* let's powerup the HD */
    
    av_ata_select_HD();
    av_ata_powerUp_HD();
        
    /* getting info from HD */
    if((res=av_ata_identify(&hd_info))<0)
    {
         printk("[av3xx-IDE] error reading disk info (err:%d)\n",res);
         return -1;
    }
    
    printk("[av3xx-IDE] reading HD info: %s (%s|%s)\n",hd_info.model,hd_info.firmware,hd_info.serial);
    
    /* getting partitions info */
    if(av_ata_readMBR(&part_list[0])<0)
    {
         printk("[av3xx-IDE] error reading partition info\n");
         return -1;
    }
    
    /* register to blkdev */
    if((res = register_blkdev(AV_IDE_MAJOR,"av_ide",&av_ide_bdops)) < 0)
    {
        printk("[av3xx-IDE] error registering ide blkdev with major: %d (err:%d)\n",AV_IDE_MAJOR,res);
        return -1;
    } 
    
    read_ahead[AV_IDE_MAJOR] = 8;
    
    av_ide_sizes=kmalloc( (AV_IDE_MAXNRDEV << AV_IDE_SHIFT) * sizeof(int),GFP_KERNEL);
    if (!av_ide_sizes)
        goto failAvIdeInit;
    memset(av_ide_sizes, 0, (AV_IDE_MAXNRDEV << AV_IDE_SHIFT) * sizeof(int));
    
    blk_size[MAJOR_NR] = av_ide_gendisk.sizes = av_ide_sizes;
    
    av_ide_partitions = kmalloc( (AV_IDE_MAXNRDEV << AV_IDE_SHIFT) * sizeof(struct hd_struct), GFP_KERNEL);
    if (!av_ide_partitions)
        goto failAvIdeInit;
    memset(av_ide_partitions, 0, (AV_IDE_MAXNRDEV << AV_IDE_SHIFT) * sizeof(struct hd_struct));
      
    av_ide_gendisk.part=av_ide_partitions;
        
    av_ide_gendisk.next=gendisk_head;
    gendisk_head=&av_ide_gendisk;
    
    blk_init_queue(BLK_DEFAULT_QUEUE(AV_IDE_MAJOR),&av_ide_request);
    
    register_disk(&av_ide_gendisk,MKDEV(AV_IDE_MAJOR,0<<AV_IDE_SHIFT),4,&av_ide_bdops,-1);
    if(av3xx_get_connected_module()==AV_MODULE_CFC)
    {
        
        register_disk(&av_ide_gendisk,MKDEV(AV_IDE_MAJOR,1<<AV_IDE_SHIFT),4,&av_ide_bdops,-1);
    }
    
    /* register to module driver */    
    if(!av3xx_module_register_action(&avcf_module_actions,AV_MODULE_CFC))
    {
        printk("[av3xx-IDE] error registering to module driver for CF reader\n");
    }
    
    if(!av3xx_module_register_action(&avcf_reader_module_actions,AV_MODULE_CFC_R))
    {
        printk("[av3xx-IDE] error registering to module driver for CF\n");
    } 
    
    av3xx_hd_launchTimer();
    
    return 0;
    
failAvIdeInit:
    return -1;
}

//static void __exit av3xx_ide_exit(void)
void av3xx_ide_exit(void)
{
    
}

/*module_init(av3xx_ide_init);
module_exit(av3xx_ide_exit);

MODULE_AUTHOR("Christophe THOMAS  <oxygen77@free.fr>");
MODULE_DESCRIPTION("Harddisk and Compact flash driver for linav (Archos Av3XX) http://linav.sf.net");
MODULE_LICENSE("GPL");
*/
