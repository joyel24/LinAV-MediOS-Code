/*
* graphics.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

void (*graphics8_Sprite_routines[3]) = {graphics8_1bit_Sprite, graphics8_2bit_Sprite, graphics8_4bit_Sprite };

void graphics8_DrawPixel(int color, int x, int y, struct graphicsBuffer * buff)
{
    OUTB(color,getOffset(x,y,buff,char));
}

int graphics8_ReadPixel(int x, int y, struct graphicsBuffer * buff)
{
    return INB(getOffset(x,y,buff,char));
}

void graphics8_DrawRect(int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
    int i,j,k;

    char * offset=getOffset(x,y,buff,char);
    
    graphics8_DrawHorizLine(color, width,offset);
            
    offset+=buff->width;
    
    for(j=1;j<height-1;j++)
    {
        OUTB(color,offset);
        OUTB(color,offset+width-1);        
        offset+=buff->width;
    }
    
    graphics8_DrawHorizLine(color, width,offset);
}

void graphics8_FillRect(int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
    int j;

    char * offset=getOffset(x,y,buff,char);
    
    for(j=0;j<height;j++)
    {
        graphics8_DrawHorizLine(color, width, offset);    
        offset+=buff->width;
    }
}

/* draw an horizontal line starting at (x,y) */ 
void graphics8_DrawHorizLine(int color, int width,char * offset)
{
    memset((void*)offset,(char)color,width);
}

void graphics8_DrawSprite(PALETTE * palette,SPRITE * sprite, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    /*void (*routine)(char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
    if(sprite->type == 8)
    {
        graphics8_DrawBITMAP  (sprite,trsp,x,y, buff);
    }
    else
    {
        routine=graphics8_Sprite_routines[sprite->type];
        routine(sprite->data,sprite->width,sprite->height,sprite->bpline,palette,trsp,x,y,buff);
    }*/
    
    int i,j,index;
    char * dest=getOffset(x,y,buff,char);
    char * src=(char*)sprite->data;
    
    for(j=0;j<sprite->height;j++)
    {
    	for(i=0;i<sprite->width;i++)
        {
            index=INB(src+i);
            OUTB(palette[index],dest+i);
        }
        dest+=buff->width;
        src+=sprite->width; 
    }
        
}

void graphics8_DrawBITMAP  (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    int i,j,c;
    char * dest=getOffset(x,y,buff,char);
    char * src=(char*)bitmap->data;

    if(trsp != -1)
    {
        for(j=0;j<bitmap->height;j++)
        {
            for(i=0;i<bitmap->width;i++)
            {
                c=INB(src+i);
                if(c!=trsp)
                    OUTB(c,dest+i);
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
                OUTB(INB(src+i),dest+i);*/
            memcpy(dest,src,bitmap->width);
            dest+=buff->width;
            src+=bitmap->width;
        }
    }
}

void graphics8_ScrollWindowVert(int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff)
{
    int j,inc;
    
    char *src,*dest;
    
    if(scroll == 0)
        return;
        
    src=getOffset(x,y,buff,char);

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
        memcpy(dest,src,width);
        dest=dest+inc*buff->width;
        src=src+inc*buff->width;
    }
    
    if(bgColor!=-1)
    { // clear the freed zone
        for(j=0;j<scroll;j++)
        {
            graphics8_DrawHorizLine(bgColor,width,dest);
            dest+=inc*buff->width;
        }
    
    }
    
    
}

void graphics8_ScrollWindowHoriz(int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff)
{
    int j;
    char tmp[320];
    char *src,*dest,*offset;
    
    if(scroll == 0)
        return;
    
    if(bgColor!=-1)
    {
        if(RIGHT)
        {
            src=getOffset(x,y,buff,char);
            dest=src+scroll;
            offset=src;
            for(j=0;j<height;j++)
            {
                memcpy(tmp,src,width-scroll);
                memcpy(dest,tmp,width-scroll);
                graphics8_DrawHorizLine(bgColor,scroll,offset);
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
        else
        {
            x-=scroll;        
            src=getOffset(x,y,buff,char);
            dest=src;
            offset=src+width-scroll;
            src=src+scroll;
            for(j=0;j<height;j++)
            {
                memcpy(dest,src,width-scroll);
                graphics8_DrawHorizLine(bgColor,scroll,offset);
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
            src=getOffset(x,y,buff,char);
            dest=src+scroll;
            offset=src;
            for(j=0;j<height;j++)
            {
                memcpy(tmp,src,width-scroll);
                memcpy(dest,tmp,width-scroll);                
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
        else
        {
            x-=scroll;        
            src=getOffset(x,y,buff,char);
            dest=src;
            offset=src+width-scroll;
            src=src+scroll;
            for(j=0;j<height;j++)
            {
                memcpy(dest,src,width-scroll);                
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
    }    
}

void graphics8_DrawChar(struct graphicsFont * font, int color,int bg_color, int x, int y, char c, struct graphicsBuffer * buff)
{
    char * src=font->table[(int)c];
    
    unsigned int palette[2]={bg_color,color};
    
    if(src!=0)    
        graphics8_1bit_Sprite(src,font->width,font->height,font->bpline,palette,-1,x,y,buff);
}

int graphics8_GetStringSize(struct graphicsFont * font, const unsigned char *str, int *w, int *h)
{
    int ch;
    int width = 0;

    while((ch = *str++))
     {
        width += font->width;
    }

    if ( w )
        *w = width;
    if ( h )
        *h = font->height;

    return width;
}

void graphics8_DrawString(struct graphicsFont * font, int color,int bg_color, int x, int y, char * s, struct graphicsBuffer * buff)
{
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
        //tmp=(char*)tmpLine;
        /*for(i=0;i<font->bpline*len;i++)
            dest[i]=tmp[i];*/
        memcpy(dest,tmpLine,font->width*len);
        dest+=buff->width;
    }     
}

void graphics8_1bit_Sprite(char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    int i,j,k,shift,index;
    
    char * dest=getOffset(x,y,buff,char);
            
    if(trsp==-1)
        for(j=0;j<height;j++)
        {
            shift=7;
            k=0;
            for(i=0;i<width;i++)
            {
                index=INB(src+k);
                index=(index>>shift)&0x1;
                OUTB(palette[index],dest+i);
                shift--;
                if(shift<0)
                {
                    shift=7;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=bpline;
        }
    else
        for(j=0;j<height;j++)
        {
            shift=7;
            k=0;
            for(i=0;i<width;i++)
            {
                index=INB(src+k);
                index=(index>>shift)&0x1;
                if(trsp!=index)
                    OUTB(palette[index],dest+i);
                shift--;
                if(shift<0)
                {
                    shift=7;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=bpline;
        }
}

void graphics8_2bit_Sprite(char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    int i,j,k,shift,index;
    
    char * dest=getOffset(x,y,buff,char);
            
    if(trsp==-1)
        for(j=0;j<height;j++)
        {
            shift=6;
            k=0;
            for(i=0;i<width;i++)
            {
                index=INB(src+k);
                index=(index>>shift)&0x3;
                OUTB(palette[index],dest+i);
                shift-=2;
                if(shift<0)
                {
                    shift=6;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=bpline;
        }
    else
        for(j=0;j<height;j++)
        {
            shift=6;
            k=0;
            for(i=0;i<width;i++)
            {
                index=INB(src+k);
                index=(index>>shift)&0x3;
                if(trsp!=index)
                    OUTB(palette[index],dest+i);
                shift-=2;
                if(shift<0)
                {
                    shift=6;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=bpline;
        }
}

void graphics8_4bit_Sprite(char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    int i,j,k,shift,index;
    
    char * dest=getOffset(x,y,buff,char);
    if(trsp==-1)
        for(j=0;j<height;j++)
        {
            shift=4;
            k=0;
            for(i=0;i<width;i++)
            {
                index=INB(src+k);
                index=(index>>shift)&0xF;
                OUTB(palette[index],dest+i);
                shift-=4;
                if(shift<0)
                {
                    shift=4;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=bpline;
        }
    else
        for(j=0;j<height;j++)
        {
            shift=4;
            k=0;
            for(i=0;i<width;i++)
            {
                index=INB(src+k);
                index=(index>>shift)&0xF;
                if(trsp!=index)
                    OUTB(palette[index],dest+i);
                shift-=4;
                if(shift<0)
                {
                    shift=4;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=bpline;
        }
}

