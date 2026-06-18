/*
* kernel/gui/gfx/icons.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <kernel/kernel.h>
#include <sys_def/string.h>
#include <kernel/stdfs.h>
#include <kernel/malloc.h>
#include <kernel/graphics.h>
#include <kernel/icons.h>

#include <gui/icons_data.h>



char ident_str[]="AVICO";

ICON icon_list_head=NULL;

ICON icon_load(char * filename)
{
    int infile;
    int i;
    int len;
    ICON ptr;
    char * tmpF;
    char * name;
    char buff[6];
  
    /* create the filename+path */
    tmpF=(char*)malloc(sizeof(char)*(strlen(ICON_DIR)+1+strlen(filename)+1));
    if(!tmpF)
    {
        printk("[icon_load] can't create filename string\n");
        return NULL;
    }    
    sprintf(tmpF,"%s/%s",ICON_DIR,filename);
    
    /* open the icon file */    
    infile = open(tmpF, O_RDONLY);
    if(infile<0)
    {
        printk("[icon_load] can't open file '%s' (%s) for reading!\n", tmpF,filename);
        return NULL;
    }
    
    /* check if it has the correct ident. string */    
    if(read(infile,buff,5)!=5)
    {
        printk("[icon_load] bad ident (length)\n"); 
        goto err1;
    }
        
    for(i=0;i<5;i++)
    {
        if(buff[i]!=ident_str[i])
        {
            printk("[icon_load] bad ident (string)\n"); 
            goto err1;
        }
    }

    /* read strlen & create string*/
    if(read(infile,buff,1)<=0)    
    {
        printk("[icon_load] end of file reached - Step2\n"); 
        goto err1;
    }
    len=(int)buff[0];
    name=(char*)kmalloc(sizeof(char)*(len+1));    
    if(!name)
    {
        printk("[icon_load] not enough memory for name in icon structure\n"); 
        goto err1;
    }
    
    /* read icon name */    
    
    /*for(i=0;i<len;i++)
    {
        
        if(read(infile,buff,1)<=0)
        {
            printk("[icon_load] end of file reached - Step3\n"); 
            goto err2;
        }
        name[i]=buff[0];
    }*/
    if(read(infile,name,len)<len)
    {
        printk("[icon_load] end of file reached - reading name\n"); 
        goto err2;
    }
    
    name[len]='\0';
    
    /* check if an icon with the same name is not already present */
    
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            printk("[icon_load] there is already an icon with that name\n");
            kfree(name);        
            goto out;
        }
    }
    
    /* create new icon struct*/
    ptr=(ICON)kmalloc(sizeof(struct icon_elem));
    if(!ptr)
    {
        printk("[icon_load] not enough memory for icon structure\n"); 
        goto err2;
    }
    
    ptr->name=name;
    
    /* read width and height */
    if(read(infile,buff,1)<=0)
    {
        printk("[icon_load] end of file reached - Step4\n"); 
        goto err3;
    }
    ptr->bmap_data.width=(int)buff[0];
    
    if(read(infile,buff,1)<=0)
    {
        printk("[icon_load] end of file reached - Step5\n"); 
        goto err3;
    }
    ptr->bmap_data.height=(int)buff[0];
    
    /* create space for bmap data */
    ptr->data=(unsigned char *)kmalloc(sizeof(unsigned char)*ptr->bmap_data.width*ptr->bmap_data.height);
    if(!ptr->data)
    {
        printk("[icon_load] not enough memory for data in icon structure (w=%d,h=%d)\n",ptr->bmap_data.width,ptr->bmap_data.height); 
        goto err3;
    }
    
    len=ptr->bmap_data.width*ptr->bmap_data.height;
    
    printk("[icon_load] loading icon %s from %s size (%d,%d)\n",ptr->name,tmpF,ptr->bmap_data.width,ptr->bmap_data.height);
    
    /* read icon data */
    /*for(i=0;i<ptr->bmap_data.width*ptr->bmap_data.height;i++)
    {
        if(read(infile,buff,1)<=0)
        {
            printk("[icon_load] end of file reached - Step6\n"); 
            //goto err4;
        }
        ptr->data[i]=buff[0];
    }*/
    
    for(i=0;i<ptr->bmap_data.height;i++)
    {
        if(read(infile,ptr->data+ptr->bmap_data.width*i,ptr->bmap_data.width)<ptr->bmap_data.width)
        {
            printk("[icon_load] end of file reached - Step6\n"); 
            goto err4;
        }
    }
    
    /*if(read(infile,ptr->data,len)<len)
    {
        printk("[icon_load] end of file reached - reading icon data\n"); 
        goto err4;
    }*/
    
    /* fill BITMAP structure */
    ptr->bmap_data.data=(unsigned int)ptr->data;
    ptr->bmap_data.type=0;
    ptr->bmap_data.bpline=0;
    
    /* insert new icon in list */
    ptr->nxt=icon_list_head;
    icon_list_head=ptr;    
    
out:
    /* close file */
    close(infile);
    free(tmpF);
    
    return ptr;
    
err4:
    kfree(ptr->data);
err3:
    kfree(ptr);    
err2:
    kfree(name);        
err1:
    close(infile);
    free(tmpF);
    return NULL;
}

ICON icon_add(char * name,unsigned char * data,int w,int h)
{
    ICON ptr;
    printk("addIcon %s ",name);
    /* check if an icon with the same name is not already present */
    
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            printk("[icon_load] there is already an icon with that name\n");
            goto out;
        }
    }
    printk("- not defined ");
    /* create new icon struct*/
    ptr=(ICON)kmalloc(sizeof(struct icon_elem));
    if(!ptr)
    {
        printk("[icon_load] not enough memory for icon structure\n");
        goto err1;
    }
    printk("- allocated ");
    ptr->name=name;
    ptr->data=data;
    ptr->bmap_data.data=(unsigned int)ptr->data;
    ptr->bmap_data.width=w;
    ptr->bmap_data.height=h;
    ptr->bmap_data.type=0;
    ptr->bmap_data.bpline=0;
    
    /* insert new icon in list */
    ptr->nxt=icon_list_head;
    icon_list_head=ptr;
    printk("- init\n");
out:
    return ptr;

err1:
    return NULL;
}

ICON icon_get(char * name)
{
    ICON ptr;
    printk("Icon %s asked ",name);
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            printk("and found 0x%08x\n",(unsigned int)ptr);
            return ptr;
        }
    }
    printk("not found\n");
    return NULL;
}

void icon_init(void)
{
    icon_add("mediosLogo",(unsigned char*)medios_logo, 51, 10);
    icon_add("usbIcon",(unsigned char*)usb_icon, 15, 6);
    icon_add("fwExtIcon",(unsigned char*)fw_ext_icon, 15, 6);
    icon_add("cfIcon",(unsigned char*)cf_icon, 15, 6);
    icon_add("powerIcon",(unsigned char*)power_icon, 13, 6);
    icon_add("upBitmap",(unsigned char*)upArrow, 9, 9);
    icon_add("dwBitmap",(unsigned char*)dwArrow, 9, 9);
    icon_add("dirBitmap",(unsigned char*)dir, 8, 8);
    icon_add("mp3Bitmap",(unsigned char*)mp3, 8, 8);
    icon_add("textBitmap",(unsigned char*)text, 8, 8);
    icon_add("imageBitmap",(unsigned char*)image, 8, 8);
    icon_add("MsgBoxExclamationBitmap",(unsigned char*)MsgBoxExclamation, 18, 18);
    icon_add("MsgBoxQuestionBitmap",(unsigned char*)MsgBoxQuestion, 18, 18);
    icon_add("MsgBoxWarningBitmap",(unsigned char*)MsgBoxWarning, 18, 18);
    icon_add("MsgBoxInformationBitmap",(unsigned char*)MsgBoxInformation, 18, 18);
    icon_add("MsgBoxErrorBitmap",(unsigned char*)MsgBoxError, 18, 18);
}
