void graphics8SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c)
{
	outb(c,graphicsGetOffset(buffer,x,y));
}

u32  graphics8GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y)
{
	return inb(graphicsGetOffset(buffer,x,y));
}

void graphics8Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c)
{
	int i,j;
	int offset=graphicsGetOffset(buffer,x,y);
	for(j=0;j<height;j++)
	{
		for(i=0;i<width;i++)
			outb(c,offset+i);
		offset+=buffer->bytesPerLine;
	}
}

void graphics8Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{

}

void graphics8String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
				char** fontlut, u32 dx, u32 dy,char *s)
{

}

void graphics8ScrollUP(struct graphicsBuffer *buffDest,int Hup)
{

}
