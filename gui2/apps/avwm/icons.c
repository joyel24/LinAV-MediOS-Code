/*
* icons.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Zakk Roberts
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "icons.h"
#include "icons_data.h"


char ident_str[]="AVICO";

#define ICON_DIR "icons"

struct icon_elem * icon_list_head=NULL;
extern char * path;

struct icon_elem * loadIcon(char * filename)
{
    FILE * infile;
    int i;
    int str_len;
    int c;
    struct icon_elem * ptr;
    char * tmpF;
    char * name;
    
    /* create the filename+path */
    tmpF=(char*)malloc(sizeof(char)*(strlen(path)+1+strlen(ICON_DIR)+1+strlen(filename)+1));
    if(!tmpF)
    {
        printf("[loadIcon] can't create filename string\n");
        return NULL;
    }    
    sprintf(tmpF,"%s/%s/%s",path,ICON_DIR,filename);
    
    /* open the icon file */    
    infile = fopen(tmpF, "rb");
    if(!infile)
    {
        printf("[loadIcon] can't open file '%s' (%s) for reading!\n", tmpF,filename);
        return NULL;
    }
    
    /* check if it has the correct ident. string */    
    for(i=0;i<5;i++)
    {
        c=getc(infile);
        if(c==EOF)
        {
            printf("[loadIcon] end of file reached - Step1\n"); 
            goto err1;
        }
        if(c!=ident_str[i])
        {
            printf("[loadIcon] bad file ident string\n"); 
            goto err1;
        }
    }

    /* read strlen & create string*/
    c=getc(infile);
    if(c==EOF)
    {
        printf("[loadIcon] end of file reached - Step2\n"); 
        goto err1;
    }
    str_len=c;
    name=(char*)malloc(sizeof(char)*(str_len+1));    
    if(!name)
    {
        printf("[loadIcon] not enough memory for name in icon structure\n"); 
        goto err1;
    }
    
    /* read icon name */
    for(i=0;i<str_len;i++)
    {
        c=getc(infile);
        if(c==EOF)
        {
            printf("[loadIcon] end of file reached - Step3\n"); 
            goto err2;
        }
        name[i]=c;
    }
    name[i]='\0';
    
    /* check if an icon with the same name is not already present */
    
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            printf("[loadIcon] there is already an icon with that name\n");
            free(name);        
            goto out;
        }
    }
    
    /* create new icon struct*/
    ptr=(struct icon_elem *)malloc(sizeof(struct icon_elem));
    if(!ptr)
    {
        printf("[loadIcon] not enough memory for icon structure\n"); 
        goto err2;
    }
    
    ptr->name=name;
    
    /* read width and height */
    c=getc(infile);
    if(c==EOF)
    {
        printf("[loadIcon] end of file reached - Step4\n"); 
        goto err3;
    }
    ptr->bmap_data.width=c;
    c=getc(infile);
    if(c==EOF)
    {
        printf("[loadIcon] end of file reached - Step5\n"); 
        goto err3;
    }
    ptr->bmap_data.height=c;
    
    /* create space for bmap data */
    ptr->data=(unsigned char *)malloc(sizeof(unsigned char)*ptr->bmap_data.width*ptr->bmap_data.height);
    if(!ptr->data)
    {
        printf("[loadIcon] not enough memory for data in icon structure (w=%d,h=%d)\n",ptr->bmap_data.width,ptr->bmap_data.height); 
        goto err3;
    }
    
    printf("[loadIcon] loading icon %s from %s size (%d,%d)\n",ptr->name,tmpF,ptr->bmap_data.width,ptr->bmap_data.height);
    
    /* read icon data */
    for(i=0;i<ptr->bmap_data.width*ptr->bmap_data.height;i++)
    {
        c=getc(infile);
        if(c==EOF)
        {
            printf("[loadIcon] end of file reached - Step6\n"); 
            goto err4;
        }
        ptr->data[i]=c;
    }
    
    /* fill BITMAP structure */
    ptr->bmap_data.data=(unsigned int)ptr->data;
    ptr->bmap_data.type=0;
    ptr->bmap_data.bpline=0;
    
    /* insert new icon in list */
    ptr->nxt=icon_list_head;
    icon_list_head=ptr;    
    
out:
    /* close file */
    fclose(infile);
    free(tmpF);
    
    return ptr;
    
err4:
    free(ptr->data);    
err3:
    free(ptr);    
err2:
    free(name);        
err1:
    fclose(infile);
    free(tmpF);
    return NULL;
}

struct icon_elem * addIcon(char * name,unsigned char * data,int w,int h)
{
    struct icon_elem * ptr;
    
    /* check if an icon with the same name is not already present */
    
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            printf("[loadIcon] there is already an icon with that name\n");
            goto out;
        }
    }
    
    /* create new icon struct*/
    ptr=(struct icon_elem *)malloc(sizeof(struct icon_elem));
    if(!ptr)
    {
        printf("[loadIcon] not enough memory for icon structure\n"); 
        goto err1;
    }
    
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

out:
    return ptr;

err1:
    return NULL;
}

struct icon_elem * getIcon(char * name)
{
    struct icon_elem * ptr;
    printf("Icon %s asked ",name);
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            printf("and found 0x%08x\n",(unsigned int)ptr);
            return ptr;
        }
    }
    printf("not found\n");
    return NULL;
}

void iniIcon(void)
{
    addIcon("linavLogo",(unsigned char*)linav_logo, 65, 10);
    addIcon("usbIcon",(unsigned char*)usb_icon, 15, 6);
    addIcon("fwExtIcon",(unsigned char*)fw_ext_icon, 15, 6);
    addIcon("cfIcon",(unsigned char*)cf_icon, 15, 6);
    addIcon("powerIcon",(unsigned char*)power_icon, 13, 6);
    addIcon("upBitmap",(unsigned char*)upArrow, 9, 9);
    addIcon("dwBitmap",(unsigned char*)dwArrow, 9, 9);
    addIcon("dirBitmap",(unsigned char*)dir, 8, 8);
    addIcon("mp3Bitmap",(unsigned char*)mp3, 8, 8);
    addIcon("textBitmap",(unsigned char*)text, 8, 8);
    addIcon("imageBitmap",(unsigned char*)image, 8, 8);
    addIcon("MsgBoxExclamationBitmap",(unsigned char*)MsgBoxExclamation, 18, 18);
    addIcon("MsgBoxQuestionBitmap",(unsigned char*)MsgBoxQuestion, 18, 18);
    addIcon("MsgBoxWarningBitmap",(unsigned char*)MsgBoxWarning, 18, 18);
    addIcon("MsgBoxInformationBitmap",(unsigned char*)MsgBoxInformation, 18, 18);
    addIcon("MsgBoxErrorBitmap",(unsigned char*)MsgBoxError, 18, 18);
}
