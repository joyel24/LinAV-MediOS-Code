/* 
*   kernel/fs/bflat.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/stdfs.h>

#include <kernel/malloc.h>

#include <kernel/bflat.h>

#include <kernel/errors.h>

#define swap_val(x) (                \
            ((x>>24) & 0x000000FF) |   \
            ((x>> 8) & 0x0000FF00) |   \
            ((x<< 8) & 0x00FF0000) |   \
            ((x<<24) & 0xFF000000)     \
                    )
                    
#define GET_VAL_FLAT(ptr)                                       \
    ({                                                          \
        unsigned long __val;                                    \
        unsigned char * __p=(unsigned char *)ptr;               \
        __val = __p[0] | __p[1]<<8 | __p[2]<<16 | __p[3]<<24;   \
        __val;                                                  \
    })

#define PUT_VAL_FLAT(ptr,val)                                   \
    ({                                                          \
        unsigned char * __p=(unsigned char *)ptr;               \
        *__p++ = val;                                           \
        *__p++ = val>>8;                                        \
        *__p++ = val>>16;                                       \
        *__p++ = val>>24;                                       \
    })

#define max(a,b)  (a<b?b:a)

#define NB_LIB    1

#define FLAT_FLAG_RAM       1
#define FLAT_FLAG_PIC       2

//#define DEBUG_FLAT

#ifdef DEBUG_FLAT
#define FLAT_PRINT(s...)   printk(s)
#else
#define FLAT_PRINT(s...)
#endif
                   
MED_RET_T load_bflat (const char * fname)
{
    int fd_bflat;
    int ret,i;
    struct bflat_header header;
    
    unsigned long text_pos,data_pos;
    unsigned long text_len,data_len,bss_len,extra_len;
    unsigned long start_code;
    unsigned long * reloc_table;
    
    int (*run_flat)(int argc,char**argv);
    
    
    fd_bflat = open(fname,O_RDONLY);
    
    if(fd_bflat<0)
    {
        printk("[load_bflat] Can't open file %s\n",fname);
        return -MED_ENOENT;
    }
    
    if((ret=read(fd_bflat,(void*)&header,sizeof(struct bflat_header)))<sizeof(struct bflat_header))
    {
        printk("[load_bflat] Can't read completly the header (read %d)\n",ret);
        close(fd_bflat);
        return -MED_EIO;
    }
    
    if(strncmp(header.magic,"bFLT",4))
    {
        header.magic[4]=0;
        printk("[load_bflat] Wrong magic (%s)\n",header.magic);
        close(fd_bflat);
        return -MED_ERROR;
    }
        
    /* Processing header data */
    
    header.rev          = swap_val(header.rev);
    header.entry        = swap_val(header.entry);
    header.data_start   = swap_val(header.data_start);
    header.data_end     = swap_val(header.data_end);
    header.bss_end      = swap_val(header.bss_end);
    header.stack_size   = swap_val(header.stack_size);
    header.reloc_start  = swap_val(header.reloc_start);
    header.reloc_count  = swap_val(header.reloc_count);
    header.flags        = swap_val(header.flags);
    
    text_len=header.data_start;
    data_len=header.data_end-header.data_start;
    bss_len=header.bss_end-header.data_end;
    
    extra_len = max(bss_len+header.stack_size,header.reloc_count*sizeof(unsigned long));
    
    printk("[load_bflat] loading %s: bFLAT:v%d text(%08x) data(%08x) bss(%08x) stack(%08x) relocs:%x\n",fname,header.rev,
                   text_len, data_len,bss_len,header.stack_size,header.reloc_count);
                   
    printk("[load_bflat] flags: %08x\n",header.flags);

    text_pos = (unsigned long)malloc((long)(text_len+data_len+extra_len+NB_LIB*sizeof(unsigned long)));

    if(!text_pos)
    {
        printk("[load_bflat] can't alloc enough mem space (%08x needed)\n",
            text_len+data_len+extra_len+NB_LIB*sizeof(unsigned long));
        close(fd_bflat);
        return -MED_ENOMEM;
    }
    
    data_pos=text_pos+header.data_start+NB_LIB*sizeof(unsigned long);
    reloc_table=(unsigned long *)(text_pos+header.reloc_start+NB_LIB*sizeof(unsigned long));
    start_code=text_pos+sizeof(struct bflat_header);
    
    printk("[load_bflat] text_pos=%08x start_code=%08x data_pos=%08x reloc_table=%08x\n",
        text_pos,start_code,data_pos,reloc_table);
    
    lseek(fd_bflat, 0, SEEK_SET);
    
    ret = read(fd_bflat,(void*)text_pos,text_len);
    
    if(ret<text_len)
    {
        printk("[load_bflat] can't read text section (ret=%d)\n",ret);
        free((void*)text_pos);
        close(fd_bflat);
        return -MED_EIO;
    }
    
    lseek(fd_bflat, header.data_start, SEEK_SET);
    
    ret = read(fd_bflat,(void*)data_pos,data_len+header.reloc_count*sizeof(unsigned long));
    
    if(ret<(data_len+header.reloc_count*sizeof(unsigned long)))
    {
        printk("[load_bflat] can't read data+remoc section (ret=%d)\n",ret);
        free((void*)text_pos);
        close(fd_bflat);
        return -MED_EIO;
    }
    
    text_len -= sizeof(struct bflat_header);
    
    if(header.flags & FLAT_FLAG_PIC)
    {
        unsigned long * reloc_point;
        unsigned long addr;
        i=0;
        FLAT_PRINT("[load_bflat] GOTPIC relocations \n");
        for(reloc_point=(unsigned long*)data_pos;(*reloc_point)!=0xFFFFFFFF;reloc_point++)
        {
            FLAT_PRINT("%d-%08x: %08x",i,reloc_point,*reloc_point);
            if(*reloc_point)
            {
                if(*reloc_point<text_len)
                    addr=*reloc_point+start_code;
                else
                    addr=*reloc_point-text_len+data_pos;
                *reloc_point=addr;
                FLAT_PRINT(" => %08x(%08x)",addr,*reloc_point);
            }
            FLAT_PRINT("\n");
        }
    }
    
    FLAT_PRINT("[load_bflat] std relocations \n");
    
    for(i=0;i<header.reloc_count;i++)
    {
        unsigned long addr,reloc_point;
        reloc_point=swap_val(reloc_table[i]);
        
        FLAT_PRINT("%d(%08x): rp=%08x",i,&reloc_table[i],reloc_point);
        
        if(reloc_point<text_len)
            reloc_point+=start_code;
        else
            reloc_point=reloc_point-text_len+data_pos;
            
        FLAT_PRINT(" rp-real=%08x",reloc_point);
        
        addr=GET_VAL_FLAT(reloc_point);
        
        if(!(header.flags & FLAT_FLAG_PIC))
            addr=swap_val(addr);
            
        FLAT_PRINT(" val=%08x",addr);
        
        if(addr<text_len)
            addr+=start_code;
        else
            addr=addr-text_len+data_pos;
        
        FLAT_PRINT(" val-rp=%08x",addr);            
            
        PUT_VAL_FLAT(reloc_point,addr);
        
        FLAT_PRINT(" (%08x)\n",*(unsigned long*)reloc_point);
    }

    
    run_flat=(int (*)(int ,char**))header.entry+text_pos;
    FLAT_PRINT("[load_bflat] about to launch: %08x\n",run_flat);
    do_bkpt();
    ret = run_flat(0,NULL);

    free((void*)text_pos);
    
    return MED_OK;
}
