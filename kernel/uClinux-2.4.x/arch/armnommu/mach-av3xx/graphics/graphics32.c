void graphics32Sprite1(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics32Sprite2(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics32Sprite4(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics32Sprite8(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics32Sprite16(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics32Sprite32(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);

void (*graphics32SpriteRoutines[6]) = {
				graphics32Sprite1, graphics32Sprite2,
				graphics32Sprite4, graphics32Sprite8,
				graphics32Sprite16,graphics32Sprite32
};

void graphics32SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c)
{
	outl(c,graphicsGetOffset(buffer,x,y));
}

u32  graphics32GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y)
{
	return inl(graphicsGetOffset(buffer,x,y));
}

void graphics32Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c)
{
	int offset=graphicsGetOffset(buffer,x,y);
	int i,j;

	for(j=0;j<height;j++)
	{
		for(i=0;i<width*4;i+=4)
		{
			outl(c,offset+i);
		}
		offset+=buffer->bytesPerLine;
	}
}

void graphics32Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{
		void (*routine)(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)=
			graphics32SpriteRoutines[buffSrc->bitsPerPixelShift];
		routine(buffer,x,y,buffSrc);
}

void graphics32Sprite1(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{
	int offsetDest=graphicsGetOffset(buffer,x,y);
	int offsetSrc=buffSrc->offset;

	int i,j,k,shift,val;

	for(j=0;j<buffSrc->height;j++)
	{
		shift=7;
		k=0;
		for(i=0;i<buffSrc->width*4;i+=4)
		{
			val=inb(offsetSrc+k);
			val=val>>shift;
			val&=0x1;
			if(val!=buffSrc->transparent)
				outl(buffSrc->pallette32[val],offsetDest+i);
			shift--;
			if(shift<0)
			{
				shift=7;
				k++;
			}
		}
		offsetDest+=buffer->bytesPerLine;
		offsetSrc+=buffSrc->bytesPerLine;
	}
}

void graphics32Sprite2(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{
	int offsetDest=graphicsGetOffset(buffer,x,y);
	int offsetSrc=buffSrc->offset;

	int i,j,k,shift,val;

	for(j=0;j<buffSrc->height;j++)
	{
		shift=6;
		k=0;
		for(i=0;i<buffSrc->width*4;i+=4)
		{
			val=inb(offsetSrc+k);
			val=val>>shift;
			val&=0x3;
			if(val!=buffSrc->transparent)
				outl(buffSrc->pallette32[val],offsetDest+i);
			shift-=2;
			if(shift<0)
			{
				shift=6;
				k++;
			}
		}
		offsetDest+=buffer->bytesPerLine;
		offsetSrc+=buffSrc->bytesPerLine;
	}
}

void graphics32Sprite4(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{
	int offsetDest=graphicsGetOffset(buffer,x,y);
	int offsetSrc=buffSrc->offset;

	int i,j,k,shift,val;

	for(j=0;j<buffSrc->height;j++)
	{
		shift=4;
		k=0;
		for(i=0;i<buffSrc->width*4;i+=4)
		{
			val=inb(offsetSrc+k);
			val=val>>shift;
			val&=0xF;
			if(val!=buffSrc->transparent)
				outl(buffSrc->pallette32[val],offsetDest+i);
			shift-=4;
			if(shift<0)
			{
				shift=4;
				k++;
			}
		}
		offsetDest+=buffer->bytesPerLine;
		offsetSrc+=buffSrc->bytesPerLine;
	}
}

void graphics32Sprite8(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{

}

void graphics32Sprite16(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{

}

void graphics32Sprite32(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)
{
	int offsetDest=graphicsGetOffset(buffer,x,y);
	int offsetSrc=buffSrc->offset;

	int i,j;

	for(j=0;j<buffSrc->height;j++)
	{
		for(i=0;i<buffSrc->width*4;i+=4)
			outl(inl(offsetSrc+i),offsetDest+i);
		offsetDest+=buffer->bytesPerLine;
		offsetSrc+=buffSrc->bytesPerLine;
	}
}

void graphics32String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
		char** fontlut, u32 dx, u32 dy,char *s)
{

}

void graphics32ScrollUP(struct graphicsBuffer *buffDest,int Hup)
{
	int i,j;
	int offSrc=graphicsGetOffset(buffDest,0,Hup);
	int offDest=graphicsGetOffset(buffDest,0,0);
	int nbRow=(buffDest->height/Hup);
	for(j=0;j<(nbRow-1)*(Hup);j++)
	{
		for(i=0;i<buffDest->width*4;i+=4)
			outl(inl(offSrc+i),offDest+i);
		offSrc+=buffDest->bytesPerLine;
		offDest+=buffDest->bytesPerLine;
	}

	offDest=graphicsGetOffset(buffDest,0,(nbRow-1)*(Hup));
	for(j=0;j<Hup;j++)
	{
		for(i=0;i<buffDest->width*4;i+=4)
			outl(0x00800080,offDest+i);
		offDest+=buffDest->bytesPerLine;
	}
}
