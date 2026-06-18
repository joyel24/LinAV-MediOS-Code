static uint32 spr_written[240*(256+16) / 32];
#define SPR_WRITTEN(ptr) (spr_written[(ptr) / 32] & (1 << (ptr & 31)))
#define SET_SPR_WRITTEN(ptr) spr_written[(ptr) / 32] |= (1 << (ptr & 31))

INLINE void ppu_renderBGLine(uint8 *buffer)
{
   uint8 *buf_ptr, *tile_ptr, *attrib_ptr;
   uint32 refresh_vaddr, attrib_base;
   uint32 x_tile, y_tile, attrib_shift;
   uint32 ppu_Latch_RenderScreen_on;

   uint32 bg_offset;
   uint8 col_row;
   uint8 tile_index;
   uint8 attrib;
   uint8 *data_ptr;

   if (0 == (ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON))
   {
      memset(buffer, 0, 256+16);
      return;
   }

   buf_ptr = buffer + (8/* - ppuTileXOfs*/);
   refresh_vaddr = 0x2000 + (ppuVAddr & 0x0fff);
   y_tile = (ppuVAddr >> 5) & 0x1f;
   x_tile = ppuVAddr & 0x1f;
   
   //MMC5
   ppu_Latch_RenderScreen_on = 0;
   if (ppu_Latch_RenderScreen) ppu_Latch_RenderScreen_on=ppu_Latch_RenderScreen(1,0);

   tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
   attrib_base = (refresh_vaddr & 0x2c00) + 0x3c0 + ((y_tile & 0x1c) << 1);
   attrib_ptr = (uint8*)(&Vnes.PPUPageIndex[(attrib_base + (x_tile >> 2))>>10][(attrib_base + (x_tile >> 2))&0x3ff]);
   attrib_shift = (x_tile & 2) + ((y_tile & 2) << 1);
   attrib = *attrib_ptr++;


   col_row = ((attrib >> attrib_shift) & 3) << 2;
   bg_offset = bg_pattern_table_addr;
   bg_offset += ((ppuVAddr >> 12) & 7);         

   if ((!ppu_Latch_RenderScreen_on)&&(!ppu_Latch_FDFE)&&(!ppu_Latch_Address))
   {   	
   	switch (x_tile&3)
   	{
   		case 3:goto left_start_x3_D;
   		case 2:goto left_start_x2_D;
   		case 1:goto left_start_x1_D;   	
   	}
   	while (1)
   	{   	
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x1_D:
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;    
      		attrib_shift ^= 2;
      		col_row = ((attrib >> attrib_shift) & 3) << 2;
   	left_start_x2_D:
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x3_D:
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;      
      		attrib = *attrib_ptr++;                   
      		attrib_shift ^= 2;
      		col_row = ((attrib >> attrib_shift) & 3) << 2;
  	}    
  	x_tile = ppuVAddr & 0x1f;
  	if ((x_tile>0)||ppuTileXOfs)
  	{
		x_tile++;	            
            
            refresh_vaddr &= 0xffe0;
        	refresh_vaddr ^= 0x0400; // flip low nametable bit        	        	
        	
        	attrib_base ^= 0x0400;

        	tile_ptr = (uint8*)&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff];
        	attrib_ptr = (uint8*)&Vnes.PPUPageIndex[(attrib_base)>>10][(attrib_base)&0x3ff];
        
        	attrib = *attrib_ptr++;         	
        	attrib_shift ^= 2; 
        	col_row = ((attrib >> attrib_shift) & 3) << 2;            	                           
        
        	while (1)
   		{
	      		tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		
      			tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		         	      		
         		attrib_shift ^= 2; 
         		col_row = ((attrib >> attrib_shift) & 3) << 2;         	
         		tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		
      			tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		         	         	
         		attrib = *attrib_ptr++;         	
         		attrib_shift ^= 2; 
         		col_row = ((attrib >> attrib_shift) & 3) << 2;            	                           
      		}
  	}   	
   }
   else
   if (ppu_Latch_RenderScreen_on)
   {
   	switch (x_tile&3)
   	{
	   	case 3:goto left_start_x3_B;
	   	case 2:goto left_start_x2_B;
	   	case 1:goto left_start_x1_B;   	
	}
   	while (1)
   	{   	
   		//MMC5
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;		
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[refresh_vaddr>>10][refresh_vaddr&0x3ff]);
    	tile_index = *tile_ptr++;            
      		
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;      
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;
   	left_start_x1_B:
   		//MMC5      		      		
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      	tile_index = *tile_ptr++;            
      		
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;      
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;		    
   	left_start_x2_B:
   		//MMC5      		
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      	tile_index = *tile_ptr++;            
      		
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;      
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;
   	left_start_x3_B:
   		//MMC5
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      	tile_index = *tile_ptr++;            
      	
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;      			
  	}    
  	x_tile = ppuVAddr & 0x1f;
  	if ((x_tile>0)||ppuTileXOfs)
  	{
		x_tile++;                        
       	refresh_vaddr ^= 0x0400; // flip low nametable bit       	
      	refresh_vaddr -= 0x0020;
       	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
       	while (1)
   		{
   			//MMC5      		
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
   			tile_index = *tile_ptr++;
      			
  			data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
   			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
  			buf_ptr += 8;      		
   			x_tile--;
   			refresh_vaddr++;
   			if (!x_tile) break;      		
   			//MMC5      	
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      		tile_index = *tile_ptr++;
      			
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      		
      		x_tile--;
      		refresh_vaddr++;
      		if (!x_tile) break;      		         		      		
         		
         	//MMC5      		
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
         	tile_index = *tile_ptr++;
      			
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      		
      		x_tile--;
      		refresh_vaddr++;
      		if (!x_tile) break;
      		//MMC5
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      		tile_index = *tile_ptr++;
      			
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		         	         	         		
  	    }
  	}
   }
   else
   if (ppu_Latch_FDFE)
   {
   	switch (x_tile&3)
   	{
   		case 3:goto left_start_x3_C;
   		case 2:goto left_start_x2_C;
   		case 1:goto left_start_x1_C;   	
   	}
   	while (1)
   	{   	
      		tile_index = *tile_ptr++;                        		
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);      		
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x1_C:
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;    
      		attrib_shift ^= 2;
      		col_row = ((attrib >> attrib_shift) & 3) << 2;
   	left_start_x2_C:
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];      		
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x3_C:
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];      		
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;      
      		attrib = *attrib_ptr++;                   
      		attrib_shift ^= 2;
      		col_row = ((attrib >> attrib_shift) & 3) << 2;
  	}    
  	x_tile = ppuVAddr & 0x1f;
  	if ((x_tile>0)||ppuTileXOfs)
  	{
		x_tile++;	            
            
        refresh_vaddr &= 0xffe0;
       	refresh_vaddr ^= 0x0400; // flip low nametable bit
       	attrib_base ^= 0x0400;

       	tile_ptr = &Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff];
       	attrib_ptr = &Vnes.PPUPageIndex[(attrib_base)>>10][(attrib_base)&0x3ff];
        
       	attrib = *attrib_ptr++;         	
       	attrib_shift ^= 2; 
       	col_row = ((attrib >> attrib_shift) & 3) << 2;            	                           
        
       	while (1)
   		{
      		tile_index = *tile_ptr++;      		
      			
   			data_ptr = &Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];      			
   			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
   			//MMC2
			CHECK_MMC2((bg_offset | (tile_index<<4)));
   			buf_ptr += 8;      		
   			x_tile--;
   			if (!x_tile) break;      		
   			tile_index = *tile_ptr++;      		
      			
   			data_ptr = &Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];      			
   			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		         	      		
         	attrib_shift ^= 2; 
         	col_row = ((attrib >> attrib_shift) & 3) << 2;         	
         	tile_index = *tile_ptr++;      		
      			
      		data_ptr = &Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		
      		tile_index = *tile_ptr++;      		
      		
      		data_ptr = &Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		         	         	
         	attrib = *attrib_ptr++;         	
         	attrib_shift ^= 2; 
         	col_row = ((attrib >> attrib_shift) & 3) << 2;            	                           
      	}
  	}
  }
  else
  if (ppu_Latch_Address)
  {
	switch (x_tile&3)
   	{
	   	case 3:goto left_start_x3_A;
	   	case 2:goto left_start_x2_A;
	   	case 1:goto left_start_x1_A;   	
	}
   	while (1)
   	{   	
   		//Map96
		ppu_Latch_Address(refresh_vaddr);
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[refresh_vaddr>>10][refresh_vaddr&0x3ff]);
    	tile_index = *tile_ptr++;            
      		
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;      
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;
   	left_start_x1_A:
   		//Map96      		      		
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      	tile_index = *tile_ptr++;            
      		
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;      
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;		    
   	left_start_x2_A:
   		//Map96      		
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      	tile_index = *tile_ptr++;            
      		
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;      
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;
   	left_start_x3_A:
   		//Map96      
		col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      	tile_index = *tile_ptr++;            
      	
      	data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      	draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      	buf_ptr += 8;
      	x_tile++;
      	refresh_vaddr++;
      	if (x_tile==32) break;      			
  	}    
  	x_tile = ppuVAddr & 0x1f;
  	if ((x_tile>0)||ppuTileXOfs)
  	{
		x_tile++;                        
       	refresh_vaddr ^= 0x0400; // flip low nametable bit       	
      	refresh_vaddr -= 0x0020;
       	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
       	while (1)
   		{
   			//Map96      		
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
   			tile_index = *tile_ptr++;
      			
  			data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
   			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
  			buf_ptr += 8;      		
   			x_tile--;
   			refresh_vaddr++;
   			if (!x_tile) break;      		
   			//Map96      	
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      		tile_index = *tile_ptr++;
      			
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      		
      		x_tile--;
      		refresh_vaddr++;
      		if (!x_tile) break;
         		
         	//Map96      		
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
         	tile_index = *tile_ptr++;
      			
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      		
      		x_tile--;
      		refresh_vaddr++;
      		if (!x_tile) break;
      		//Map96      		
		 	col_row=ppu_Latch_RenderScreen(1,(refresh_vaddr) & 0x03FF) & 0x0C;
		 	tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
      		tile_index = *tile_ptr++;
      			
      		data_ptr = (uint8*)(&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], col_row);
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		         	         	         		
  	    }
  	}
   }
  
  

   /* need to blank left column? */
   if (0 == (ppuRegs[REG_CTRL1] & REGF_CTRL1_BGNOCLIP))
   {
      /*uint32*/ uint8 *buf = (/*uint32*/uint8 *) (buffer + 8 + ppuTileXOfs);
/*      ((uint32 *) buf)[0] = 0;
      ((uint32 *) buf)[1] = 0;      */
      buf[0]=0;buf[1]=0;buf[2]=0;buf[3]=0;
      buf[4]=0;buf[5]=0;buf[6]=0;buf[7]=0;
   }   

}


INLINE void ppu_renderBGLine0(uint8 *buffer)
{
   uint8 *buf_ptr, *tile_ptr;
   uint32 refresh_vaddr;
   int x_tile, y_tile;

   uint32 bg_offset;   
   uint8 tile_index;   
   uint8 *data_ptr;

   if (0 == (ppuRegs[REG_CTRL1] & REGF_CTRL1_BGON))
   {
      memset(buffer, 0, 256+16);
      return;
   }

   buf_ptr = buffer + (8/* - ppuTileXOfs*/);
   refresh_vaddr = 0x2000 + (ppuVAddr & 0x0fe0);
   y_tile = (ppuVAddr >> 5) & 0x1f;
   x_tile = ppuVAddr & 0x1f;

   tile_ptr = (uint8*)(&Vnes.PPUPageIndex[(refresh_vaddr + x_tile)>>10][(refresh_vaddr + x_tile)&0x3ff]);
   

   
   bg_offset = bg_pattern_table_addr;
   bg_offset += ((ppuVAddr >> 12) & 7);
   
   
   
   if (ppu_Latch_FDFE)
   {
   	switch (x_tile&3)
   	{
   		case 3:goto left_start_x3_C;
   		case 2:goto left_start_x2_C;
   		case 1:goto left_start_x1_C;   	
   	}
   	while (1)
   	{   	
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], 0);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x1_C:
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], 0);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;    
      		
   	left_start_x2_C:
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x3_C:
      		tile_index = *tile_ptr++;                  
      		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      		//MMC2
      		CHECK_MMC2((bg_offset | (tile_index<<4)));
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;      
      		
  	}    
  	x_tile = ppuVAddr & 0x1f;
  	if ((x_tile>0)||ppuTileXOfs)
  	{
		x_tile++;	            
            
        	refresh_vaddr ^= 0x0400; // flip low nametable bit


        	tile_ptr = (uint8*)&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff];
                
        	while (1)
   		{
	      		tile_index = *tile_ptr++;      		
      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			//MMC2
      			CHECK_MMC2((bg_offset | (tile_index<<4)));
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		
      			tile_index = *tile_ptr++;      		
      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			//MMC2
      			CHECK_MMC2((bg_offset | (tile_index<<4)));
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		         	      		
         		
         		tile_index = *tile_ptr++;      		
      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			//MMC2
      			CHECK_MMC2((bg_offset | (tile_index<<4)));
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		
      			tile_index = *tile_ptr++;
      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			//MMC2
      			CHECK_MMC2((bg_offset | (tile_index<<4)));
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		         	         	
         		
      		}
  	}
  }
  else
  {
  	switch (x_tile&3)
   	{
   		case 3:goto left_start_x3_D;
   		case 2:goto left_start_x2_D;
   		case 1:goto left_start_x1_D;   	
   	}
   	while (1)
   	{   	
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], 0);
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x1_D:
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr, (data_ptr[8]<<8)| data_ptr[0], 0);
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;    
      		
   	left_start_x2_D:
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      		buf_ptr += 8;      
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x3_D:
      		tile_index = *tile_ptr++;                        		
      		data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;      
      		
  	}    
  	x_tile = ppuVAddr & 0x1f;
  	if ((x_tile>0)||ppuTileXOfs)
  	{
		x_tile++;	            
            
        	refresh_vaddr ^= 0x0400; // flip low nametable bit


        	tile_ptr = (uint8*)&Vnes.PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff];
        	
        	while (1)
   		{
	      		tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		
      			tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		         	      		
         		
         		
         		tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		
      			tile_index = *tile_ptr++;      		      			
      			data_ptr = (uint8*)&Vnes.PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      			draw_bgtile(buf_ptr,  (data_ptr[8]<<8)| data_ptr[0], 0);
      			buf_ptr += 8;      		
      			x_tile--;
      			if (!x_tile) break;      		         	         	
         		
      		}
  	}
  }
  

   /* need to blank left column? */
   if (0 == (ppuRegs[REG_CTRL1] & REGF_CTRL1_BGNOCLIP))
   {
      /*uint32*/ uint8 *buf = (/*uint32*/uint8 *) (buffer + 8 + ppuTileXOfs);
/*      ((uint32 *) buf)[0] = 0;
      ((uint32 *) buf)[1] = 0;      */
      buf[0]=0;buf[1]=0;buf[2]=0;buf[3]=0;
      buf[4]=0;buf[5]=0;buf[6]=0;buf[7]=0;
   }   

}
    
        
__IRAM_CODE void ppu_lazy_renderBGLines(void)
{
   uint8 *buf_ptr, *tile_ptr, *attrib_ptr/*,*buf_ptr_tmp*/;
   uint32 refresh_vaddr, attrib_base;
   uint32 x_tile, y_tile, attrib_shift;

   uint32 bg_offset;
   uint8 col_row;
   uint8 tile_index;
   uint8 attrib;
   uint8 *data_ptr;
   uint8 *buffer;
   uint32 linestodraw,line;

//   char str[512];
/*   sprintf(str,"Lazy bg on %d, %d",ppuScanline,lazy_bg_start_line);   
	DrawMessage(str,0);
	gpwait(0);*/

   buffer = (uint8*)(Vnes.var.Vbuffer + lazy_bg_start_line*(256+16));

   if (0 == lazy_bg_enabled)
   {
      memset(buffer + 8, 0, (256+16)*(ppuScanline-lazy_bg_start_line));
      return;
   }

/*   	DrawMessage("got something",0);
	gpwait(0);*/
   
   /*SCANLINE START*/
   lazy_ppuVAddr = (lazy_ppuVAddr & 0xFBE0) | (lazy_ppuVAddrLatch & 0x041F);
   /**/


   while (lazy_bg_start_line<ppuScanline)
   {   	   
	   buf_ptr = buffer + (8/* - lazy_ppuTileXOfs*/);
	   refresh_vaddr = 0x2000 + (lazy_ppuVAddr & 0x0fff);
	   y_tile = (lazy_ppuVAddr >> 5) & 0x1f;
	   x_tile = lazy_ppuVAddr & 0x1f;
   
	   tile_ptr = (uint8*)(&lazy_PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff]);
	   attrib_base = (refresh_vaddr & 0x2c00) + 0x3c0 + ((y_tile & 0x1c) << 1);
	   attrib_ptr = (uint8*)(&lazy_PPUPageIndex[(attrib_base + (x_tile >> 2))>>10][(attrib_base + (x_tile >> 2))&0x3ff]);
	   attrib_shift = (x_tile & 2) + ((y_tile & 2) << 1);
	   attrib = *attrib_ptr++;


	   col_row = ((attrib >> attrib_shift) & 3) << 2;	   
	   line = ((lazy_ppuVAddr >> 12) & 7);	   
	   
	   linestodraw = MIN(8-line,ppuScanline-lazy_bg_start_line);
	   
	   bg_offset = lazy_bg_pattern_table_addr | line;
	   
	   
/*	   sprintf(str,"Line %d tile_y %d tile_x %d\nbgogs %04X Xofs %d 2draw %d\nppu %04X %04X %04X %04X\nlazy %04X %04X %04X %04X",
	   lazy_bg_start_line,y_tile,x_tile,bg_offset,lazy_ppuTileXOfs,linestodraw,
	   Vnes.PPUPageIndex[bg_offset>>10],Vnes.PPUPageIndex[(bg_offset>>10)+1],Vnes.PPUPageIndex[(bg_offset>>10)+2],Vnes.PPUPageIndex[(bg_offset>>10)+3],
	   lazy_PPUPageIndex[bg_offset>>10],lazy_PPUPageIndex[(bg_offset>>10)+1],lazy_PPUPageIndex[(bg_offset>>10)+2],lazy_PPUPageIndex[(bg_offset>>10)+3]);   
		DrawMessage(str,0);		
		gpwait(0);*/
                  
	   switch (x_tile&3)
	   {
   		case 3:goto left_start_x3_lazy_A;
   		case 2:goto left_start_x2_lazy_A;
   		case 1:goto left_start_x1_lazy_A;
   	   }
   	   while (1)
   	   {
      		tile_index = *tile_ptr++;
      		data_ptr = (uint8*)(&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
 /*     		buf_ptr_tmp=buf_ptr;      		
      		for (i=0;i<linestodraw;i++)
      		{
      			draw_bgtile(buf_ptr_tmp, (data_ptr[8|i]<<8)| data_ptr[i], col_row);
	      		buf_ptr_tmp+=256+16;
      		}*/
      		draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
      		
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x1_lazy_A:
      		tile_index = *tile_ptr++;
      		data_ptr = (uint8*)(&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
/*      		buf_ptr_tmp=buf_ptr;      		
      		for (i=0;i<linestodraw;i++)
      		{
	      		draw_bgtile(buf_ptr_tmp, (data_ptr[8|i]<<8)| data_ptr[i], col_row);
	      		buf_ptr_tmp+=256+16;
	      	}*/
      		draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
      			      	
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;
      		attrib_shift ^= 2;
      		col_row = ((attrib >> attrib_shift) & 3) << 2;
   	left_start_x2_lazy_A:
      		tile_index = *tile_ptr++;
      		data_ptr = (uint8*)(&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff]);
/*      		buf_ptr_tmp=buf_ptr;      		
      		for (i=0;i<linestodraw;i++)
      		{
      			draw_bgtile(buf_ptr_tmp,  (data_ptr[8|i]<<8)| data_ptr[i], col_row);
      			buf_ptr_tmp+=256+16;
      		}*/
      		draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
      		
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;
   	left_start_x3_lazy_A:
      		tile_index = *tile_ptr++;
      		data_ptr = (uint8*)&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
/*      		buf_ptr_tmp=buf_ptr;      		
      		for (i=0;i<linestodraw;i++)
      		{
      			draw_bgtile(buf_ptr_tmp,  (data_ptr[8|i]<<8)| data_ptr[i], col_row);
      			buf_ptr_tmp+=256+16;
      		}*/
      		draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
      		
      		buf_ptr += 8;
      		x_tile++;
      		if (x_tile==32) break;
      		attrib = *attrib_ptr++;
      		attrib_shift ^= 2;
      		col_row = ((attrib >> attrib_shift) & 3) << 2;
  	}
  	x_tile = lazy_ppuVAddr & 0x1f;
  	if ((x_tile>0)||lazy_ppuTileXOfs)
  	{
		x_tile++;
            
        refresh_vaddr &= 0xffe0;
       	refresh_vaddr ^= 0x0400; // flip low nametable bit
        	
        attrib_base ^= 0x0400;

       	tile_ptr = (uint8*)&lazy_PPUPageIndex[(refresh_vaddr)>>10][(refresh_vaddr)&0x3ff];
       	attrib_ptr = (uint8*)&lazy_PPUPageIndex[(attrib_base)>>10][(attrib_base)&0x3ff];
        
       	attrib = *attrib_ptr++;         	
       	attrib_shift ^= 2; 
       	col_row = ((attrib >> attrib_shift) & 3) << 2;            	                           
  

       	while (1)
   		{
      		tile_index = *tile_ptr++;
     		data_ptr = (uint8*)&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
      		draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
	     	
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		
      		tile_index = *tile_ptr++;      		      			
      		data_ptr = (uint8*)&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
	      	draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
	      	
      		buf_ptr += 8;      		
      		x_tile--;      		
      		if (!x_tile) break;
         	attrib_shift ^= 2; 
         	col_row = ((attrib >> attrib_shift) & 3) << 2;         	
         	
         	
         	
         	tile_index = *tile_ptr++;      		      			
      		data_ptr = (uint8*)&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
	      	draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
	      	
      		buf_ptr += 8;      		
      		x_tile--;
      		if (!x_tile) break;      		
      		tile_index = *tile_ptr++;      		      			
      		data_ptr = (uint8*)&lazy_PPUPageIndex[(bg_offset | (tile_index<<4))>>10][(bg_offset | (tile_index<<4))&0x3ff];
	      	draw_bgtileL(buf_ptr, data_ptr, col_row,linestodraw);
	      	
      		buf_ptr += 8;      		
      		x_tile--;
         	if (!x_tile) break;       		
         	attrib = *attrib_ptr++;         	
         	attrib_shift ^= 2; 
         	col_row = ((attrib >> attrib_shift) & 3) << 2;            	                                    	         	     		         	         	         	

      	}

  	}
  	
  	
   /* need to blank left column? */
   
      /*NEXT SCANLINE*/
      /*to optimize!!!*/
   lazy_bg_start_line+=linestodraw;
   while (linestodraw)
   {
		if (0 == (lazy_bgnoclip))
		{
			/*uint32 *buf = (uint32 *) (buffer + 8);            
			((uint32 *) buf)[0] = 0;
			((uint32 *) buf)[1] = 0;      	    */
			uint8 *buf = (uint8 *) (buffer + 8 + lazy_ppuTileXOfs);
    	  	buf[0]=0;buf[1]=0;buf[2]=0;buf[3]=0;
	    	buf[4]=0;buf[5]=0;buf[6]=0;buf[7]=0;

		}    
   
	    if((lazy_ppuVAddr & 0x7000) == 0x7000) /* is subtile y offset == 7? */
    	{
		      lazy_ppuVAddr &= 0x8FFF; /* subtile y offset = 0 */
		      if((lazy_ppuVAddr & 0x03E0) == 0x03A0) /* name_tab line == 29? */
		      {
		        lazy_ppuVAddr ^= 0x0800;  /* switch nametables (bit 11) */
		        lazy_ppuVAddr &= 0xFC1F;  /* name_tab line = 0 */
		      }
		      else
		      {
		        if((lazy_ppuVAddr & 0x03E0) == 0x03E0) /* line == 31? */
		        {
		          lazy_ppuVAddr &= 0xFC1F;  /* name_tab line = 0 */
		        }
		        else
		        {
		          lazy_ppuVAddr += 0x0020;
		        }
		      }
		}
		else
		{
			lazy_ppuVAddr += 0x1000; /* next subtile y offset */
		}		
		buffer += 256+16;
		linestodraw--;		
   }
  }
}
  
     
     
     

__IRAM_CODE void ppu_lazy_renderSPRLines(void)
{             
    int32 s;              // sprite #
	int32  spr_x;         // sprite coordinates
	uint32 spr_y;
	uint8* spr;           // pointer to sprite RAM entry
	uint8* p;             // draw pointer
	uint32 solid_ptr;
	int lines_to_draw;
	int x_len;
	
	uint32 priority;
	
	uint32 pattern;
	
	int32 inc_x, inc_y;   // drawing vars
	int32 start_x, end_x;
	int32 x,y;            // in-sprite coords
	
	uint8* t;
	
	
	memset(&spr_written[(256+16) / 32 * lazy_spr_start_line], 0,
		(256+16) / 8  * (ppuScanline - lazy_spr_start_line));

	/*do mmc5 stuff here*/	
	
	
	for(s = 0, spr = lazy_oamMemory.memory; s < 64; s++, spr+=4)
	{
		//spr = &spr_ram[s<<2];
		
		// get y coord
		spr_y = spr[0]+1;
		
		// on current scanline region?
		if((spr_y > ppuScanline) || ((spr_y+(obj_height)) <= lazy_spr_start_line))
			continue;
 		
		// get x coord
		spr_x = spr[3];
		
		start_x = 0;
		end_x = 8;
		
		// clip right
		if((spr_x + 7) > 255)
		{
			end_x -= ((spr_x + 7) - 255);
		}
		
		// clip left
		if((spr_x < 8) && (!lazy_objnoclip))
		{
			if(0 == spr_x) continue;
			start_x += (8 - spr_x);
		}

		/*check mmc2 here*/
		/*
		int name_addr = spr[1] << 4;
		if(((name_addr) & 0x0FC0) == 0x0FC0) {
			if((((name_addr) & 0x0FF0) == 0x0FD0) || (((name_addr) & 0x0FF0) == 0x0FE0)) {
				parent_NES->mapper->PPU_Latch_FDFE(name_addr);
			}
		}
		memcpy(lazy_tile_banks, PPU_tile_banks, sizeof(lazy_tile_banks));
		*/
		
		// clip top
		if (spr_y < lazy_spr_start_line) {
			y = lazy_spr_start_line - spr_y;
		} else {
			y = 0;
		}
		lines_to_draw = MIN(obj_height - y, ppuScanline - spr_y);

		// calc offsets into buffers
		p = Vnes.var.Vbuffer + (256+16) * (spr_y + y) + spr_x + start_x+8 + lazy_ppuTileXOfs;

		//solid = spr_written + NES_BACKBUF_WIDTH * (spr_y + y) + SIDE_MARGIN + spr_x + start_x;
		solid_ptr = (256+16) * (spr_y + y) + spr_x + start_x;

		// flip horizontally?
		x_len = end_x - start_x;;
		if(spr[2] & 0x40) // yes
		{
			inc_x = -1;
			start_x = (8-1) - start_x;
			end_x = (8-1) - end_x;
		}
		else
		{
			inc_x = 1;
		}

		// flip vertically?
		if(spr[2] & 0x80) // yes
		{
			y = (obj_height-1) - y;
			inc_y = -1;
		}
		else
		{
			inc_y = 1;
		}
		
		// get priority bit
		priority = spr[2] & 0x20;

		while (lines_to_draw) {

			uint32 tile_addr = spr[1] << 4;
			if(obj_height==16) 
			{
    			tile_addr&=0xFE0;
				if(spr[1] & 0x01) 
				{					
					tile_addr |= 0x1000;
				}
				if(y > 7) tile_addr += 8;
			} 
			else 
			{
				tile_addr += lazy_spr_pattern_table_addr;
			}

			// read 16bits = 2bits x 8pixels
			//uint8 *t = TILE(tile_addr) + TILE_OFFSET(line);
			t = (uint8*)( lazy_PPUPageIndex[tile_addr>>10] + (tile_addr&0x3FF) + y);
			pattern = (t[8] << 13) | (t[0]<<5);

			if (pattern) 
			{
			    pattern|=(spr[2] & 0x03)<<3;
				for(x = start_x; x != end_x; x += inc_x)
				{
					//uint8 col = 0x00;
					uint8 col;
					
					// if a sprite has drawn on this pixel, don't draw anything
					//if(!(*solid))
					if(!SPR_WRITTEN(solid_ptr))
					{
						col=patternTOpix[pattern|x];
						
						if (col) 
						{													
							//*solid = 1;
							SET_SPR_WRITTEN(solid_ptr);
							if(priority)
							{
								if(!(*p)) // BG color
								{
									*p = col|0x10;
								}
							}
							else
							{
								*p = col|0x10;
							}
						}
					}
					
					p++;
					//solid++;
					solid_ptr++;
				}
				p += 256+16 - x_len;
				//solid += NES_BACKBUF_WIDTH - x_len;
				solid_ptr += 256+16 - x_len;
			} else {
				p += 256+16;
				solid_ptr += 256+16;
			}
			lines_to_draw--;
			y += inc_y;
			
//			if (lazy_objnoclip)
		}		
	}
}
