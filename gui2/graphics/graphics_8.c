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
	unsigned int val;
	char * offset=getOffset(x,y,buff,char);
	
	
	
	for(i=0;i<width;i++)
		OUTB(color,offset+i);
			
	offset+=buff->width;
	
	for(j=1;j<height-1;j++)
	{
		OUTB(color,offset);
		OUTB(color,offset+width);		
		offset+=buff->width;
	}
	
	for(i=0;i<width;i++)
		OUTB(color,offset+i);
}

void graphics8_FillRect(int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
	int i,j,k,l,m;
	unsigned int val;
	char * offset=getOffset(x,y,buff,char);
	/*val= color<<24 | color<<16 | color<<8 | color;
	for(j=0;j<height;j++)
	{
		m=4-(((int)offset)%4);
		if(m<4)
		{
			for(i=0;i<m;i++)
				OUTB(color,offset+i);	
		}
		else
			m=0;		
		for(i=0;i<((width-m)/4);i++)
		{
			OUTL(val,offset+(i*4)+m);
			for(l=0;l<10;l++);
		}
		k=i*4;
		for(l=0;l<((width-m)%4);l++)
			OUTB(color,offset+k+l+m);
		offset+=buff->width;
	}*/
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
			OUTB(color,offset+i);
		offset+=buff->width;
	}
}

void graphics8_DrawSprite(SPRITE * sprite, PALETTE * palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
	void (*routine)(char * src, int width, int height, int bpline, PALETTE *palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
	if(sprite->type == 8)
	{
		graphics8_DrawBITMAP  (sprite,trsp,x,y, buff);
	}
	else
	{
		routine=graphics8_Sprite_routines[sprite->type];
		routine(sprite->data,sprite->width,sprite->height,sprite->bpline,palette,trsp,x,y,buff);
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
			for(i=0;i<bitmap->width;i++)
				OUTB(INB(src+i),dest+i);
			dest+=buff->width;
			src+=bitmap->width;
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

