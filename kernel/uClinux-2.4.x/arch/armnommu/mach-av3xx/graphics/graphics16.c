void graphics16SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c)
{
	outw(c,graphicsGetOffset(buffer,x,y));
}

u32  graphics16GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y)
{
	return inw(graphicsGetOffset(buffer,x,y));
}

void graphics16Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c)
{	          
	int i,j;
	int offset=graphicsGetOffset(buffer,x,y);
	
	 /* For word writes... | cc | cc | */
	c&=0xFFFF;
	c|=c<<16;  
	
	for(j=0;j<height;j++)
	{
		i=0;
		while(i<width-2)
		{
			outl(c,offset+i);
			i+=4;
		}
		if(i<width)
			outw(c,offset+i);
		offset+=buffer->bytesPerLine;
	}
}

void graphics16Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{

}

void graphics16String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
				char** fontlut, u32 dx, u32 dy,char *s)
{

}

void graphics16ScrollUP(struct graphicsBuffer *buffDest,int Hup)
{

}
