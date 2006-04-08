/* 
*   kernel/gfx/graphics_32.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <sys_def/string.h>

#include <kernel/graphics.h>

void         graphics32_DrawPixel         (unsigned int color, int x, int y, struct graphicsBuffer * buff);
unsigned int graphics32_ReadPixel         (int x, int y, struct graphicsBuffer * buff);
void         graphics32_DrawRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics32_FillRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics32_DrawChar          (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y,
                                                unsigned char c, struct graphicsBuffer * buff);
void         graphics32_DrawSprite        (unsigned int * palette, SPRITE * sprite, unsigned int trsp,int x, int y,
                                                struct graphicsBuffer * buff);
void         graphics32_DrawBITMAP        (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void         graphics32_ScrollWindowVert  (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP,
                                                struct graphicsBuffer * buff);
void         graphics32_ScrollWindowHoriz (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT,
                                                struct graphicsBuffer * buff);
void         graphics32_DrawHLine         (unsigned int color, int x, int y, int width, struct graphicsBuffer * buff);
void         graphics32_DrawVLine         (unsigned int color, int x, int y, int height, struct graphicsBuffer * buff);
void         graphics32_DrawHorizLine (unsigned int color, int width,unsigned int * offset);
void         graphics32_DrawString    (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y,
                                            unsigned char * s, struct graphicsBuffer * buff);

struct graphics_operations g32ops =  {
	drawPixel         : graphics32_DrawPixel,
	readPixel         : graphics32_ReadPixel,
	drawRect          : graphics32_DrawRect,
	fillRect          : graphics32_FillRect,
	drawSprite        : graphics32_DrawSprite,
	drawChar          : graphics32_DrawChar,
	drawBITMAP        : graphics32_DrawBITMAP,
	drawString        : graphics32_DrawString,
	scrollWindowVert  : graphics32_ScrollWindowVert,
	scrollWindowHoriz : graphics32_ScrollWindowHoriz,
        drawHLine         : graphics32_DrawHLine,
        drawVLine         : graphics32_DrawVLine
};

void graphics32_DrawPixel(unsigned int color, int x, int y, struct graphicsBuffer * buff)
{
    outl(color,getOffset(x,y,buff,unsigned int));
}

unsigned int graphics32_ReadPixel(int x, int y, struct graphicsBuffer * buff)
{
    return inl(getOffset(x,y,buff,unsigned int));
}

void graphics32_DrawRect(unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
    int i;

    unsigned int * offset=getOffset(x,y,buff,unsigned int);
    
    graphics32_DrawHorizLine(color, width, offset);
            
    offset+=buff->width;
    
    for(i=1;i<height-1;i++)
    {
        outl(color,offset);
        outl(color,offset+width-1);        
        offset+=buff->width;
    }
    
    graphics32_DrawHorizLine(color, width,offset);
}

void graphics32_FillRect(unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
    int j;

    unsigned int * offset=getOffset(x,y,buff,unsigned int);
    
    for(j=0;j<height;j++)
    {
        graphics32_DrawHorizLine(color, width, offset);    
        offset+=buff->width;        
    }
    
}

void graphics32_DrawHLine(unsigned int color, int x, int y, int width, struct graphicsBuffer * buff)
{
    unsigned int * offset=getOffset(x,y,buff,unsigned int);
    graphics32_DrawHorizLine(color,width,offset);
}

void graphics32_DrawVLine(unsigned int color, int x, int y, int height, struct graphicsBuffer * buff)
{
    int i;
    unsigned int * offset=getOffset(x,y,buff,unsigned int);
    for(i=0;i<height;i++)
    {
        outl(color,offset);
        offset+=buff->width;
    }
}

/* draw an horizontal line starting at (x,y) */ 
void graphics32_DrawHorizLine(unsigned int color, int width,unsigned int * offset)
{
    int i;
    for(i=0;i<width;i++)   
        outl(color,offset++);
}

void graphics32_DrawSprite(unsigned int * palette,SPRITE * sprite, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{    
    int i,j,index;
    unsigned int * dest=getOffset(x,y,buff,unsigned int);
    unsigned int * src=(unsigned int*)sprite->data;
    if(trsp != -1)
    {
        for(j=0;j<sprite->height;j++)
        {
            for(i=0;i<sprite->width;i++)
            {
                index=inl(src+i);
                if(palette[index]!=trsp)
                	outl(palette[index],dest+i);
            }
            dest+=buff->width;
            src+=sprite->width; 
        }
    }
    else
    {
        for(j=0;j<sprite->height;j++)
        {
            for(i=0;i<sprite->width;i++)
            {
                index=inl(src+i);
                outl(palette[index],dest+i);
            }
            dest+=buff->width;
            src+=sprite->width; 
        }    
    }
        
}

void graphics32_DrawBITMAP(BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    int i,j,c;
    unsigned int * dest=getOffset(x,y,buff,unsigned int );
    unsigned int * src=(unsigned int *)bitmap->data;

    if(trsp != -1)
    {
        for(j=0;j<bitmap->height;j++)
        {
            for(i=0;i<bitmap->width;i++)
            {
                c=inl(src+i);
                if(c!=trsp)
                    outl(c,dest+i);
            }
            dest+=buff->width;
            src+=bitmap->width;
        }
    }
    else
    {
        for(j=0;j<bitmap->height;j++)
        {
            /*for(i=0;i<bitmap->width;i++)
                outl(inl(src+i),dest+i);*/
            memcpy(dest,src,bitmap->width*4);
            dest+=buff->width;
            src+=bitmap->width;
        }
    }
}

void graphics32_ScrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff)
{
    int i,j,inc;
    
    unsigned int *src,*dest;
    
    if(scroll == 0)
        return;
        
    src=getOffset(x,y,buff,unsigned int);

    if(UP)
    {
        inc=1;
        dest=src;
        src=src+scroll*buff->width;
    }
    else
    {
        inc=-1;
        dest=src+height*buff->width;
        src=dest-scroll*buff->width;
    }
    
    for(j=0;j<(height-scroll);j++)
    {
        for(i=0;i<width;i++)
           outl(inl(src+i),dest+i);
        dest=dest+inc*buff->width;
        src=src+inc*buff->width;
    }
    
    if(bgColor!=-1)
    { // clear the freed zone
        for(j=0;j<scroll;j++)
        {
            graphics32_DrawHorizLine(bgColor,width,dest);
            dest+=inc*buff->width;
        }
    
    }
    
    
}

void graphics32_ScrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff)
{
    int i,j;
    unsigned int tmp[320];
    unsigned int *src,*dest,*offset;
    
    if(scroll == 0)
        return;
    
    if(bgColor!=-1)
    {
        if(RIGHT)
        {
            src=getOffset(x,y,buff,unsigned int);
            dest=src+scroll;
            offset=src;
            for(j=0;j<height;j++)
            {
#warning optimization needed here in Scroll 32 horizontal - left
                for(i=0;i<(width-scroll);i++)
                    outl(inl(src+i),tmp+i);
                for(i=0;i<(width-scroll);i++)
                    outl(inl(tmp+i),dest+i);
                graphics32_DrawHorizLine(bgColor,scroll,offset);
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
        else
        {
            x-=scroll;        
            src=getOffset(x,y,buff,unsigned int);
            dest=src;
            offset=src+width-scroll;
            src=src+scroll;
            for(j=0;j<height;j++)
            {
                for(i=0;i<(width-scroll);i++)
                    outl(inl(src+i),dest+i);
                graphics32_DrawHorizLine(bgColor,scroll,offset);
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
    }
    else
    {
        if(RIGHT)
        {
            src=getOffset(x,y,buff,unsigned int);
            dest=src+scroll;
            offset=src;
            for(j=0;j<height;j++)
            {
#warning optimization needed here in Scroll 32 horizontal -right
                for(i=0;i<(width-scroll);i++)
                    outl(inl(src+i),tmp+i);
                for(i=0;i<(width-scroll);i++)
                    outl(inl(tmp+i),dest+i);
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
        else
        {
            x-=scroll;        
            src=getOffset(x,y,buff,unsigned int);
            dest=src;
            offset=src+width-scroll;
            src=src+scroll;
            for(j=0;j<height;j++)
            {
                for(i=0;i<(width-scroll);i++)
                    outl(inl(src+i),dest+i);               
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
    }    
}

void graphics32_DrawChar(struct graphicsFont * font, unsigned int color, unsigned int bg_color, int x, int y, unsigned char c, struct graphicsBuffer * buff)
{
    int i,j,k,shift,index;
    
    unsigned char * src=font->table[(int)c];
    unsigned int * dest=getOffset(x,y,buff,unsigned int);
    
    unsigned int palette[2]={bg_color,color};
    
    if(src!=0) 
    {
    	for(j=0;j<font->height;j++)
        {
            shift=7;
            k=0;
            for(i=0;i<font->width;i++)
            {
                index=inl(src+k);
                index=(index>>shift)&0x1;
                outl(palette[index],dest+i);
                shift--;
                if(shift<0)
                {
                    shift=7;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=font->bpline;
        }
    }   
}

void graphics32_DrawString(struct graphicsFont * font, unsigned int color, unsigned int bg_color, int x, int y, unsigned char * s, struct graphicsBuffer * buff)
{
    #if 0
    int i,val,j,k;
    unsigned int color_table[16];
    char * src;
    char * str;
    char * dest;
    char * tmp;
    int len;
    char tmpLine[320+4];
    
    dest=getOffset(x,y,buff,char);
    len=strlen(s);
    
    for(i=0;i<0x10;i++)
    {
        color_table[i]  = (i&0x8?color:bg_color);
        color_table[i] |= (i&0x4?color:bg_color) << 8;
        color_table[i] |= (i&0x2?color:bg_color) << 16 ;
        color_table[i] |= (i&0x1?color:bg_color) << 24;
    }

    for(j=0;j<font->height;j++)
    {
        str=s;
        k=0;        
        while(*str)
        {
            src=font->table[(int)(*str)];
            if(src!=0)
            {
                for(i=0;i<font->bpline;i++)
                {
                    val=(int)(src[i+j*font->bpline]);
                    memcpy(&tmpLine[k],&color_table[(val>>4)&0xF],4);
                    k+=4;
                    memcpy(&tmpLine[k],&color_table[val&0xF],4);
                    k+=4;
                }
                if((font->width&0x3)!=0)
                    k=k-(4-(font->width&0x3));
            }
            else
            {
                /* to be done */
                /* add bg color to do blank*/
            }
            str++;
        }
        memcpy(dest,tmpLine,font->width*len);
        dest+=buff->width;
    }     
    #endif
    while(*s)
    {
        graphics32_DrawChar(font,color,bg_color,x,y, *s, buff);
        x+=font->width;
        s++;
    }
}

