/* 
*   kernel/fs/med.c
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
#include <sys_def/types.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/kfile.h>

#include <kernel/malloc.h>

#include <kernel/med.h>

void load_med(char * file_name)
{
    int fd,ret,i,res;
    
    int (*run_flat)(int argc,char**argv);
    
    elf_hdr header;
    
    section_hdr * sections;
    char * sections_name;
    
    int text_section=0;
    int data_section=0;
    int rodata_section=0;
    int core_section=0;
    
    int rel_text_section=0;
    int rel_core_section=0;
    
    char * text_start;
    char * ro_data_start;
    char * data_start;
    char * core_start=(char*)&_iram_end+0x10;
    
    int nb_ent;
    uint32_t entry;
    
    fd = kfopen(file_name,O_RDONLY);
    if(fd<0)
    {
        printk("[load_med] Can't open file %s\n",file_name);
        return ;
    }
    
    if((ret=kfread(fd,(void*)&header,sizeof(elf_hdr)))<sizeof(elf_hdr))
    {
        printk("[load_med] Can't read completly the header (read %d)\n",ret);
        goto exit_point;
    }
    
    if(header.e_ident[0] != 0x7F || header.e_ident[1] != 0x45 || header.e_ident[2] != 0x4c || header.e_ident[3] != 0x46)
    {
        header.e_ident[4]=0;
        printk("[load_med] Wrong magic (%s)\n",header.e_ident);
        goto exit_point;
    }
    
    sections = (section_hdr *)malloc(sizeof(section_hdr)*header.e_shnum);
    if(!sections)
    {
        printk("Error can't malloc an array of section header of %d elements\n",header.e_shnum);
        goto exit_point;
    }
            
    for(i=0;i<header.e_shnum;i++)
    {
        klseek (fd,header.e_shoff+i*header.e_shentsize,SEEK_SET);
        res=kfread(fd,(void*)&(sections[i]),sizeof(section_hdr));
    }
    
    klseek (fd,sections[header.e_shstrndx].sh_offset,SEEK_SET);
    
    sections_name = (char*)malloc(sizeof(char)*sections[header.e_shstrndx].sh_size);
    if(!sections_name)
    {
        printk("Error can't malloc an array of char of %d elements for sections' names\n",sections[header.e_shstrndx].sh_size);
        goto exit_point1;
    }
    
    res=kfread(fd,(void*)sections_name,sizeof(char)*sections[header.e_shstrndx].sh_size);
    
    for(i=0;i<header.e_shnum;i++)
    {
        /* trying to find sections*/
        if(!strcmp(&sections_name[sections[i].sh_name],".text"))
        {
            text_section=i;
            printk("text section from %x to %x\n",sections[i].sh_addr,sections[i].sh_addr+sections[i].sh_size);
        }
        else if(!strcmp(&sections_name[sections[i].sh_name],".core"))
        {
            core_section=i;
            printk("core section from %x to %x\n",sections[i].sh_addr,sections[i].sh_addr+sections[i].sh_size);
        }
        else if(!strcmp(&sections_name[sections[i].sh_name],".rodata"))
        {
            rodata_section=i;
            printk("rodata section from %x to %x\n",sections[i].sh_addr,sections[i].sh_addr+sections[i].sh_size);
        }
        else if(!strcmp(&sections_name[sections[i].sh_name],".data"))
        {
            data_section=i;
            printk("data section from %x to %x\n",sections[i].sh_addr,sections[i].sh_addr+sections[i].sh_size);
        }
        else
            printk("unknown section %d: %s\n",i,&sections_name[sections[i].sh_name]);
    }
    
    for(i=0;i<header.e_shnum;i++)
    {
        if(sections[i].sh_type == 9)
        {
            printk("found rel ");
            if(sections[i].sh_info == text_section)
            {
                rel_text_section = i;
                printk("for text\n");
            }
            else if(sections[i].sh_info == core_section)
            {
                rel_core_section = i;
                printk("for core\n");
            }
            else
                printk("-Error %d is a rel for %d (%s)\n",i,sections[i].sh_info,&sections_name[sections[sections[i].sh_info].sh_name]);
        }        
    }     
        
    /* creating text and data section */
    
    
    if((0x7000-(unsigned int)core_start)<sections[core_section].sh_size)
    {
        printk("Can't get a buffer for CORE: avail=%x need %x\n",0x7000-(unsigned int)core_start,sections[core_section].sh_size);
        goto exit_point2;
    }
    
    
    text_start = (char*)malloc(sections[text_section].sh_size+sections[data_section].sh_size+sections[rodata_section].sh_size);
    
    if(!text_start)
    {
        printk("Can't get a buffer in SDRAM size: %x\n",sections[text_section].sh_size+sections[data_section].sh_size+sections[rodata_section].sh_size);
        goto exit_point2;
    }
    else
        printk("buffer in SDRAM created; size: %x\n",sections[text_section].sh_size+sections[data_section].sh_size+sections[rodata_section].sh_size);
        
    data_start = text_start + sections[text_section].sh_size;
    ro_data_start = data_start + sections[data_section].sh_size;
    
    if(sections[text_section].sh_size)
    {
        klseek (fd,sections[text_section].sh_offset,SEEK_SET);
        res = kfread(fd,(void*)text_start,(size_t)sections[text_section].sh_size);
        printk("TEXT load at 0x%x, read %x/%x\n",text_start,res,(size_t)sections[text_section].sh_size);
    }
    else
        printk("no TEXT loaded (size==0)\n");
        
    if(sections[data_section].sh_size)
    {
        klseek (fd,sections[data_section].sh_offset,SEEK_SET);
        res = kfread(fd,(void*)data_start,sections[data_section].sh_size);
        printk("DATA load at 0x%x, read %x/%x\n",data_start,res,sections[data_section].sh_size);
    }
    else
        printk("no DATA loaded (size==0)\n");
        
    if(sections[rodata_section].sh_size)
    {
        printk("RODATA load ");
        klseek (fd,sections[rodata_section].sh_offset,SEEK_SET);
        res = kfread(fd,(void*)ro_data_start,sections[rodata_section].sh_size);
        printk("at 0x%x, read %x/%x\n",ro_data_start,res,sections[rodata_section].sh_size);
    }
    else
        printk("no RODATA loaded (size==0)\n");
    
    if(sections[core_section].sh_size)
    {
        printk("CORE load ");
        klseek (fd,sections[core_section].sh_offset,SEEK_SET);
        res = kfread(fd,(void*)core_start,sections[core_section].sh_size);
        printk("at 0x%x, read %x/%x\n",core_start,res,sections[core_section].sh_size);
    }
    else
        printk("no CORE loaded (size==0)\n");
     
    /* now processing relocs */
      
    if(rel_text_section)
    {
        rel_entry rel_data;
        uint32_t addr;
        uint32_t content;
        nb_ent = sections[rel_text_section].sh_size/sections[rel_text_section].sh_entsize;
        
        printk("we'll have to process %d relocs\n",nb_ent);
        for(i=0;i<nb_ent;i++)
        {
            klseek(fd,sections[rel_text_section].sh_offset+i*sizeof(rel_entry),SEEK_SET);
            kfread(fd,(void*)&rel_data,sizeof(rel_entry));
            /* only considering type 2 rel */
            if(ELF32_R_TYPE(rel_data.r_info) == 0x2)
            {
                addr = rel_data.r_offset-sections[text_section].sh_addr+(uint32_t)text_start;                
                content = inl(addr);
                
                if(sections[text_section].sh_addr <= content && (sections[text_section].sh_addr+sections[text_section].sh_size)>content)
                {
                    printk("rel from text to text\n");
                    outl(content-sections[text_section].sh_addr+(uint32_t)text_start,addr);                    
                }
                else if(sections[data_section].sh_addr <= content && (sections[data_section].sh_addr+sections[data_section].sh_size)>content)
                {
                    printk("rel from text to data\n");
                    outl(content-sections[data_section].sh_addr+(uint32_t)data_start,addr);                    
                }
                else if(sections[rodata_section].sh_addr <= content && (sections[rodata_section].sh_addr+sections[rodata_section].sh_size)>content)
                {
                    printk("rel from text to rodata\n");
                    outl(content-sections[rodata_section].sh_addr+(uint32_t)ro_data_start,addr);                    
                }
                else if(sections[core_section].sh_addr <= content && (sections[core_section].sh_addr+sections[core_section].sh_size)>content)
                {
                    printk("rel from text to core\n");
                    outl(content-sections[core_section].sh_addr+(uint32_t)core_start,addr);                    
                }
                else
                    printk("didn't find correct section: 0x%x from 0x%x\n",content,addr);
            }
            //printk("REL %x: off:0x%x SYM: 0x%x Type: 0x%x\n",i,rel_data.r_offset,ELF32_R_SYM(rel_data.r_info),ELF32_R_TYPE(rel_data.r_info));       
        }
    }
    else
        printk("no rel in text\n");
    
    if(rel_core_section)
    {
        rel_entry rel_data;
        uint32_t addr;
        uint32_t content;
        nb_ent = sections[rel_core_section].sh_size/sections[rel_core_section].sh_entsize;
        
        printk("we'll have to process %d relocs\n",nb_ent);
        for(i=0;i<nb_ent;i++)
        {
            klseek(fd,sections[rel_core_section].sh_offset+i*sizeof(rel_entry),SEEK_SET);
            kfread(fd,(void*)&rel_data,sizeof(rel_entry));
            /* only considering type 2 rel */
            if(ELF32_R_TYPE(rel_data.r_info) == 0x2)
            {
                addr = rel_data.r_offset-sections[core_section].sh_addr+(uint32_t)core_start;                
                content = inl(addr);
                
                if(sections[text_section].sh_addr <= content && (sections[text_section].sh_addr+sections[text_section].sh_size)>content)
                {
                    printk("rel from core to text\n");
                    outl(content-sections[text_section].sh_addr+(uint32_t)text_start,addr);                    
                }
                else if(sections[data_section].sh_addr <= content && (sections[data_section].sh_addr+sections[data_section].sh_size)>content)
                {
                    printk("rel from core to data\n");
                    outl(content-sections[data_section].sh_addr+(uint32_t)data_start,addr);                    
                }
                else if(sections[rodata_section].sh_addr <= content && (sections[rodata_section].sh_addr+sections[rodata_section].sh_size)>content)
                {
                    printk("rel from core to rodata\n");
                    outl(content-sections[rodata_section].sh_addr+(uint32_t)ro_data_start,addr);                    
                }
                else if(sections[core_section].sh_addr <= content && (sections[core_section].sh_addr+sections[core_section].sh_size)>content)
                {
                    printk("rel from core to data\n");
                    outl(content-sections[core_section].sh_offset+(uint32_t)core_start,addr);                    
                }
                else
                    printk("didn't find correct section: 0x%x from 0x%x\n",content,addr);
            }
            //printk("REL %x: off:0x%x SYM: 0x%x Type: 0x%x\n",i,rel_data.r_offset,ELF32_R_SYM(rel_data.r_info),ELF32_R_TYPE(rel_data.r_info));       
        }
    }
    else
        printk("no rel in core\n");
           
    if(sections[text_section].sh_addr <= header.e_entry && (sections[text_section].sh_addr+sections[text_section].sh_size)>header.e_entry)
    {
        entry = header.e_entry - sections[text_section].sh_addr + (uint32_t)text_start;
        printk("entry point in text: 0x%x\n",entry);
    }
    else if(sections[core_section].sh_addr <= header.e_entry && (sections[core_section].sh_addr+sections[core_section].sh_size)>header.e_entry)
    {
        entry = header.e_entry - sections[core_section].sh_addr + (uint32_t)core_start;
        printk("entry point in core: 0x%x\n",entry);
    }
    else
    {
        printk("Entry not in text or core\n");
        goto exit_point3;
    }
         
    run_flat = (int (*)(int ,char**))entry;
    
    printk("calling app\n");      
    
    run_flat(0,NULL);
    
    printk("back from app\n");           
exit_point3:    
    free(text_start);
exit_point2:    
    free(sections_name);
exit_point1:
    free(sections);
exit_point:
    kfclose(fd);
    
}
