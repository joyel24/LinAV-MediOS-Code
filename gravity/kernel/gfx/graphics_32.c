/* 
*   kernel/gfx/graphics_32.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <sys_def/string.h>

#include <kernel/kgraphics.h>

void graphics32_DrawPixel (COLOR color, int x, int y, GFX_CONTEXT* pCtx)
{
	outl (color, getCtxOffset (x, y, pCtx));
}

COLOR graphics32_ReadPixel (int x, int y, GFX_CONTEXT* pCtx)
{
	return inl (getCtxOffset (x, y, pCtx));
}

void graphics32_DrawVLine (COLOR color, int x, int y, int height, GFX_CONTEXT* pCtx)
{
	int i;
	COLOR* offset = getCtxOffset (x, y, pCtx);
	for(i=0;i<height;i++)
	{
		outl (color, offset);
		offset += (pCtx->delta >> 2);
	}
}

/* draw an horizontal line starting at (x,y) */
void graphics32_DrawHorizLine (COLOR color, int width, COLOR* offset)
{
	int i;
	for(i=0;i<width;i++)
		outl (color,offset++);
}

void graphics32_DrawHLine (COLOR color, int x, int y, int width, GFX_CONTEXT* pCtx)
{
	COLOR* offset = getCtxOffset (x, y, pCtx);
	graphics32_DrawHorizLine (color, width, offset);
}

void graphics32_DrawRect (COLOR color, int x, int y, int width, int height, GFX_CONTEXT* pCtx)
{
	int i;

	COLOR* offset = getCtxOffset (x, y, pCtx);

	graphics32_DrawHorizLine (color, width, offset);

	offset += (pCtx->delta >> 2);

	for(i=1;i<height-1;i++)
	{
		outl (color,offset);
		outl (color,offset+width-1);
		offset += (pCtx->delta >> 2);
	}

	graphics32_DrawHorizLine (color, width, offset);
}

void graphics32_ProtectContextRect (int *x, int *y, int *width, int *height, GFX_CONTEXT* pCtx)
{
	if (*x < 0)
	{
		*width += *x;
		*x = 0;
	}

	if (*y < 0)
	{
		*height += *y;
		*y = 0;
	}

	if (*x + *width > pCtx->w)
	{
		*width = pCtx->w - *x;
	}

	if (*y + *height > pCtx->h)
	{
		*height = pCtx->h - *y;
	}
}

void graphics32_FillRect (COLOR color, int x, int y, int width, int height, GFX_CONTEXT* pCtx)
{
	int j;

	graphics32_ProtectContextRect (&x, &y, &width, &height, pCtx);

	COLOR* offset = getCtxOffset (x, y, pCtx);

	for(j=0;j<height;j++)
	{
		graphics32_DrawHorizLine (color, width, offset);
		offset += (pCtx->delta >> 2);
	}
}

#define ABS(v) ((v)>=0?(v):-(v))

int GetCohenSutherlandOutCode (int v, int vmin, int vmax)
{
	if (v > vmax)
		return 1;
	else
	if (v < vmin)
		return -1;

	return 0;
}

int CohenSutherlandLineClipping (int* x0, int* y0, int* x1, int* y1, int xmin, int xmax, int ymin, int ymax)
{
	int outx0 = GetCohenSutherlandOutCode ( *x0, xmin, xmax);
	int outy0 = GetCohenSutherlandOutCode ( *y0, ymin, ymax);
	int outx1 = GetCohenSutherlandOutCode ( *x1, xmin, xmax);
	int outy1 = GetCohenSutherlandOutCode ( *y1, ymin, ymax);

	while (1)
	{
		if ((outx0 == 0) && (outx1 == 0) && (outy0 == 0) && (outy1 == 0))
			return 1;

		if ((outx0 == outx1) && (outy0 == outy1))
			return 0;

		if ((outx0 != 0) && (outx0 == outx1))
			return 0;

		if ((outy0 != 0) && (outy0 == outy1))
			return 0;

		if (outy0 > 0)
			{ *x0 = *x0 + (*x1 - *x0) * (ymax - *y0) / (*y1 - *y0); *y0 = ymax; }
		else
		if (outy0 < 0)
			{ *x0 = *x0 + (*x1 - *x0) * (ymin - *y0) / (*y1 - *y0); *y0 = ymin; }
		else
		if (outx0 > 0)
			{ *y0 = *y0 + (*y1 - *y0) * (xmax - *x0) / (*x1 - *x0); *x0 = xmax; }
		else
		if (outx0 < 0)
			{ *y0 = *y0 + (*y1 - *y0) * (xmin - *x0) / (*x1 - *x0); *x0 = xmin; }
		else
		if (outy1 > 0)
			{ *x1 = *x0 + (*x1 - *x0) * (ymax - *y0) / (*y1 - *y0); *y1 = ymax; }
		else
		if (outy1 < 0)
			{ *x1 = *x0 + (*x1 - *x0) * (ymin - *y0) / (*y1 - *y0); *y1 = ymin; }
		else
		if (outx1 > 0)
			{ *y1 = *y0 + (*y1 - *y0) * (xmax - *x0) / (*x1 - *x0); *x1 = xmax; }
		else
		if (outx1 < 0)
			{ *y1 = *y0 + (*y1 - *y0) * (xmin - *x0) / (*x1 - *x0); *x1 = xmin; }

		outx0 = GetCohenSutherlandOutCode (*x0, xmin, xmax);
		outy0 = GetCohenSutherlandOutCode (*y0, ymin, ymax);
		outx1 = GetCohenSutherlandOutCode (*x1, xmin, xmax);
		outy1 = GetCohenSutherlandOutCode (*y1, ymin, ymax);
	}

	return 0;
}

void graphics32_DrawLine (COLOR color, int x1, int y1, int x2, int y2, GFX_CONTEXT* pCtx)
{
	if (!CohenSutherlandLineClipping (&x1, &y1, &x2, &y2, 0, pCtx->w-1, 0, pCtx->h-1))
		return;

	int numpixels;
	int i;
	int deltax, deltay;
	int d, dinc1, dinc2;
	int x, xinc1, xinc2;
	int y, yinc1, yinc2;

	if(x1==x2)
	{
		if(y1>y2)
		{
			i=y1;
			y1=y2;
			y2=i;
		}
		graphics32_DrawVLine (color, x1, y1, y2-y1+1, pCtx);
		return;
	}

	if(y1==y2)
	{
		if(x1>x2)
		{
			i=x1;
			x1=x2;
			x2=i;
		}
		graphics32_DrawHLine (color, x1, y1, x2-x1+1, pCtx);
		return;
	}

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);

	if(deltax >= deltay)
	{
		numpixels = deltax;
		d = 2 * deltay - deltax;
		dinc1 = deltay * 2;
		dinc2 = (deltay - deltax) * 2;
		xinc1 = 1;
		xinc2 = 1;
		yinc1 = 0;
		yinc2 = 1;
	}
	else
	{
		numpixels = deltay;
		d = 2 * deltax - deltay;
		dinc1 = deltax * 2;
		dinc2 = (deltax - deltay) * 2;
		xinc1 = 0;
		xinc2 = 1;
		yinc1 = 1;
		yinc2 = 1;
	}
	numpixels++; /* include endpoints */

	if(x1 > x2)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}

	if(y1 > y2)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	x = x1;
	y = y1;

	COLOR* pOutput = getCtxOffset (x, y, pCtx);
	int inc1 = xinc1 + yinc1 * (pCtx->delta >> 2);
	int inc2 = xinc2 + yinc2 * (pCtx->delta >> 2);

	for (i=0; i<numpixels; i++)
	{
		outl (color, pOutput);

		if(d < 0)
		{
			d += dinc1;
			pOutput += inc1;
		}
		else
		{
			d += dinc2;
			pOutput += inc2;
		}
	}
}

void graphics32_DrawAALine (
	GFX_CONTEXT* pCtx,
	int x1, int y1,
	int x2, int y2,
	int opacity, COLOR color,
	int set_last_pixel)
{
	if (!CohenSutherlandLineClipping (&x1, &y1, &x2, &y2, 0, pCtx->w-1, 0, pCtx->h-1))
		return;

	int dX, dY;
	int Xinc, Yinc;
	int X, Y;
	int DeltaX, DeltaY;
	int i, j;
	int D;
	int Xs, Ys;
	int Cp;
	int I_p, I_s;

	dX = x2 - x1;
	dY = y2 - y1;

	DeltaX = ABS(dX);
	DeltaY = ABS(dY);

	if (dX > 0)
		Xinc = +1;
	else
		Xinc = -1;

	if (dY > 0)
		Yinc = +1;
	else
		Yinc = -1;

	X = x1;
	Y = y1;

	unsigned char* ptr = (unsigned char*)getCtxOffset (X, Y, pCtx);
	unsigned char* pColor = (unsigned char*)&color;
	for (j=0;j<4;j++)
		ptr[j] = (pColor[j] * opacity + ptr[j] * (255 - opacity)) >> 8;

	int start = 1;
	if (set_last_pixel)
		start = 0;

	if (DeltaX >= DeltaY)
	{
		D = 0;
		for (i = start; i<DeltaX; i++)
		{
			X += Xinc;
			D += DeltaY;
			if (D <= 0)
				Ys = Y - Yinc;
			else
			{
				if (D - DeltaX < 0)
				{
					if (2 * D - DeltaX <= 0)
						Ys = Y + Yinc;
					else
					{
						Ys = Y;
						Y += Yinc;
						D -= DeltaX;
					}
				}
				else
				{
					Y += Yinc;
					Ys = Y + Yinc;
					D -= DeltaX;
				}
			}
			Cp = ABS ( D * 256 / DeltaX );
			I_s = (Cp * opacity + 128) / 256;
			I_p = opacity - I_s;

			unsigned char* ptra = (unsigned char*)getCtxOffset (X, Y, pCtx);
			unsigned char* ptrb = 0;
			if ((Ys >= 0) && (Ys < pCtx->h))
				ptrb = (unsigned char*)getCtxOffset (X, Ys, pCtx);
			unsigned char* pColor = (unsigned char*)&color;
			for (j=0;j<4;j++)
			{
				ptra[j] = (I_p * pColor[j] + (255 - I_p) * ptra[j]) >> 8;
				if (ptrb)
					ptrb[j] = (I_s * pColor[j] + (255 - I_s) * ptrb[j]) >> 8;
			}
		}
	}
	else
	{
		D = 0;
		for (i = start; i<DeltaY; i++)
		{
			Y += Yinc;
			D += DeltaX;
			if (D <= 0)
				Xs = X - Xinc;
			else
			{
				if (D - DeltaY < 0)
				{
					if (2 * D - DeltaY <= 0)
						Xs = X + Xinc;
					else
					{
						Xs = X;
						X += Xinc;
						D -= DeltaY;
					}
				}
				else
				{
					X += Xinc;
					Xs = X + Xinc;
					D -= DeltaY;
				}
			}
			Cp = ABS ( D * 256 / DeltaY );
			I_s = (Cp * opacity + 128) / 256;
			I_p = opacity - I_s;

			unsigned char* ptra = (unsigned char*)getCtxOffset (X, Y, pCtx);
			unsigned char* ptrb = 0;
			if ((Xs >= 0) && (Xs < pCtx->w))
				ptrb = (unsigned char*)getCtxOffset (Xs, Y, pCtx);
			unsigned char* pColor = (unsigned char*)&color;
			for (j=0;j<4;j++)
			{
				ptra[j] = (I_p * pColor[j] + (255 - I_p) * ptra[j]) >> 8;
				if (ptrb)
					ptrb[j] = (I_s * pColor[j] + (255 - I_s) * ptrb[j]) >> 8;
			}
		}
	}
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
            graphics32_DrawHorizLine((COLOR)bgColor,width,(COLOR*)dest);
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
                graphics32_DrawHorizLine((COLOR)bgColor,scroll,(COLOR*)offset);
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
                graphics32_DrawHorizLine((COLOR)bgColor,scroll,(COLOR*)offset);
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

