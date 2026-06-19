
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <ata.h>
#include <stdio.h>

//******************************include our memalloc.h***********************************
#include "avmalloc.h" //this file itself include#'s stdio.h for us if necessary
//***************************************************************************************

static int pal16a[2] = {0x0000, 0x1d1d};
static int pal16b[2] = {0x0000, 0xfafa};

//make and init structures for each font we'll be using
struct graphicsBuffer font5x8   ={0,1,5, 8, 1,0,-1,0,0,0,0,(int**)&pal16a, 0};
struct graphicsBuffer font6x10  ={0,1,6,10, 1,0,-1,0,0,0,0,(int**)&pal16b, 0};

//make structure for osd component we're using
struct graphicsBuffer screenBitmap;

void Delay(unsigned int time);

char str[80] = "";
char strblank[80] = "                                                                    ";


int main() {

    ataPowerDownHDDA();

    //-----INIT screenBitmap structure ----
    screenBitmap.offset = 0x03500000;   //typical address for bitmap1 memory
    screenBitmap.bytesPerLine = 320*2;  
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4; //always 4 for 16-bit bitmap
    screenBitmap.bitsPerPixel = 16;     //always 16 for 16-bit bitmap

    //-----INIT OSD SYSTEM -----
    //call osdInit and disable everything just to be safe
    osdInitA();
    osdSetComponentConfigA(OSD_VIDEO1, 0); osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0); osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0); osdSetComponentConfigA(OSD_CURSOR2, 0);
    //set up BITMAP1 as needed, enable it and clear screen; corresponds with screenBitmap
    osdSetComponentSizeA(OSD_BITMAP1, 640,240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03500000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_0TRANS
                                     | OSD_BITMAP_A0);
    graphicsBoxfA(&screenBitmap,0,0,320,240,0x0000); //clear bitmap area

    //Print instructions to screen and the title line
    graphicsStringA(&screenBitmap,0,0,&font6x10,std6x10_,9,0,"BGET ('AVMALLOC') Test Demo");
    graphicsStringA(&screenBitmap,0,15,&font5x8,std5x8_,5,0,"tests up to 99 malloc'ed blocks (going over causes exit)");
    graphicsStringA(&screenBitmap,0,25,&font5x8,std5x8_,5,0,"Demo uses up to 1MB at 0x03f00000 - 0x03ffffff");
    graphicsStringA(&screenBitmap,0,90,&font6x10,std6x10_,6,0,"Next allocation will be:      400 bytes");
    graphicsStringA(&screenBitmap,0,120,&font5x8,std5x8_,5,0,"RIGHT: malloc next block with current size");
    graphicsStringA(&screenBitmap,0,130,&font5x8,std5x8_,5,0,"LEFT:  free block (in order from block 0)");
    graphicsStringA(&screenBitmap,0,140,&font5x8,std5x8_,5,0,"UP:    increase size");
    graphicsStringA(&screenBitmap,0,150,&font5x8,std5x8_,5,0,"DOWN:  decrease size");
    graphicsStringA(&screenBitmap,0,160,&font5x8,std5x8_,5,0,"MENU1: calloc next block with current size (init w/0's)");
    graphicsStringA(&screenBitmap,0,170,&font5x8,std5x8_,5,0,"MENU2: realloc block with new size (in order from 1st active)");
    graphicsStringA(&screenBitmap,0,180,&font5x8,std5x8_,5,0,"  (also fills new block with previous block's contents)");
    graphicsStringA(&screenBitmap,0,190,&font5x8,std5x8_,5,0,"MENU3: show/check contents & size of blocks");
    graphicsStringA(&screenBitmap,0,200,&font5x8,std5x8_,5,0,"  (cycles thru all blocks that are currently active)");
    graphicsStringA(&screenBitmap,0,210,&font5x8,std5x8_,5,0,"ON:    fill last alloc'ed block with matching numbers");
    graphicsStringA(&screenBitmap,0,220,&font5x8,std5x8_,5,0,"  (fills block memory with # of block, for error checking)");
    graphicsStringA(&screenBitmap,0,230,&font5x8,std5x8_,5,0,"OFF:   exit");
    pal16a[1] = 0xffff;

    int key, val, change, nextsize=400, oldsize, i; //misc variables 
    int ind1 = 0, ind2 = 0, showind = 0, reallocind = 0;  //just some program counters
    char *ptr[100];  //this holds an array of up to 100 pointers that will
                    //  point to various blocks of dynamically allocated memory
                    //  obtained via bget, bgetz, bgetr
                    //These are pointers to char because I want to easily access every byte
                    //  with pointer arithmetic
    int size[100];  //will hold the size of each block we allocated

//*************Set aside ~1MB worth for dynamic use, starting at 0x03f00000****************
    //Set aside any amount of space you think you'll need for any parts of your program
    //  or libraries that use these avalloc allocation methods to obtain dynamic memory.
    //It's totally up to you where you put this, probably higher in memory is best
    //  to avoid conflicts with bootloader, video memory etc.  This is a dumb routine -- so
    //  you must avoid conflicts yourself!!!
    int *allocspace;
    allocspace = (int*)0x03f00000; //set to av300 mem addr 0x03f00000, which is 1MB below top
    bpool(allocspace,0xfffff);     //bpool initializes the area that allocations will use:
                                   //bpool(start addr, size in bytes)
                                   //You can evidently call this again later to add other
                                   //  chunks of memory if it's decided later that
                                   //  you'll need more total space for dynamic memory
//*****************************************************************************************

    while (1) {
        //--- Process av300 key presses ---
        key = buttonsGetStatusA();
        if (key & BUTTONS_AV300_RIGHT) {// Right joystick, do malloc equivalent
            ind1++;
            if (ind1>99) return; //just exit program if user tries more than 100 allocs
     //***********************************************************************************
            ptr[ind1] = (char*)bget(nextsize); //equivalent to: ptr = (char*)malloc(size)
            //note the cast to char in this and other allocation functions is only for
            //  my case here, you off course cast to whatever your pointers are pointing
            //  to, more commonly it'll be ints probably.  I wanted access to each individual
            //  byte easily for this testing so I use pointers to char type.
     //***********************************************************************************
            if (ptr[ind1]!=0) { //worked fine
                size[ind1] = nextsize;
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"malloc'ed #%d: size %d bytes at mem location 0x%x",\
                    ind1, size[ind1], ptr[ind1]);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
            }
            else { //returned 0, means there was an error
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"ERROR:  couldn't malloc #%d: size %d bytes (too big??)",\
                    ind1, nextsize);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
                ind1--;
            }
            Delay(0x1FFFF);
        }
        if (key & BUTTONS_AV300_LEFT) {//  Left joystick, do free equivalent
            ind2++;
            if (ind2>ind1) ind2=ind1; //don't allow freeing blocks that haven't been alloced
            else {
     //********************************************************************************
                brel(ptr[ind2]);  //equivalent to:  free(ptr)
     //********************************************************************************
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"freed malloc'ed space #%d! (was using %d bytes)", ind2, size[ind2]);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
                Delay(0x1FFFF);
            }
        }
        if (key & BUTTONS_AV300_UP) {//   Up joystick, increase 'size' number
            if (nextsize>100000) nextsize+=10000;
            else if (nextsize>10000) nextsize+=2000;
            else if (nextsize>1000) nextsize+=200;
            else if (nextsize>400) nextsize+=50;
            else nextsize+=10;
            sprintf(str,"Next allocation will be: %8d bytes",nextsize);
            graphicsStringA(&screenBitmap,0,90,&font6x10,std6x10_,6,0, str);
            Delay(0x8FFF);      
        }
        if (key & BUTTONS_AV300_DOWN) {// Down joystick, decrease 'size' number
            if (nextsize>100000) nextsize-=10000;
            else if (nextsize>10000) nextsize-=2000;
            else if (nextsize>1000) nextsize-=200;
            else if (nextsize>400) nextsize-=50;
            else nextsize-=10;
            if (nextsize<10) nextsize = 10;
            sprintf(str,"Next allocation will be: %8d bytes",nextsize);
            graphicsStringA(&screenBitmap,0,90,&font6x10,std6x10_,6,0, str);
            Delay(0x8FFF);
        }
        if (key & BUTTONS_AV300_MENU1) {//F1 button, do calloc equivalent
            ind1++;
            if (ind1>99) return; //just exit program if user tries more than 100 allocs
     //***********************************************************************************
            ptr[ind1] = (char*)bgetz(nextsize); //equivalent to: ptr = (char*)calloc(size)
     //***********************************************************************************
            if (ptr[ind1]!=0) { //worked fine
                size[ind1] = nextsize;
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"calloc'ed #%d: size %d bytes at mem location 0x%x",\
                    ind1, size[ind1], ptr[ind1]);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
            }
            else { //returned 0, means there was an error
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"ERROR:  couldn't calloc #%d: size %d bytes (too big??)",\
                    ind1, nextsize);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
                ind1--;
            }
            Delay(0x1FFFF);
        }
        if (key & BUTTONS_AV300_MENU2) {//F2 button, do realloc equivalent
          if (ind2>=ind1) {  //no currently active blocks, so don't do anything
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,"--no active blocks--");
            Delay(0x1FFFF);
          }
          else {
            reallocind++;
            if (reallocind<=ind2) reallocind=ind1;
            if (reallocind>ind1) reallocind=ind2+1;
     //********************************************************************************
            ptr[reallocind] = (char*)bgetr(ptr[reallocind], nextsize);
                //equivalent to: ptr = (char*)realloc(ptr, newsize)
     //********************************************************************************
            if (ptr[reallocind]!=0) { //worked fine
                oldsize = size[reallocind]; size[reallocind] = nextsize;
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"realloc'ed #%d: new sz %d bytes @ new mem loc. 0x%x",\
                    reallocind, size[reallocind], ptr[reallocind]);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
            }
            else { //returned 0, means there was an error
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
                sprintf(str,"ERROR:  couldn't realloc #%d: new size %d bytes (too big??)",\
                    reallocind, nextsize);
                graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
                ind1--;
            }
            //fill in the full new size with the same numbers that original had
            //  we only need to update the extra size portion, cause existing contents when
            //  using realloc are always preserved.
            if (oldsize<nextsize) { //only necessary if new size is larger...
                for (i=oldsize; i<=nextsize; i++) 
                    *(ptr[reallocind] + i - 1) = *(ptr[reallocind]);
            }
            Delay(0x1FFFF);
          }
        }
        if (key & BUTTONS_AV300_MENU3) {//F3 button, show contents/size
          if (ind2>=ind1) {  //no currently active blocks, so don't do anything
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,"--no active blocks--");
            Delay(0x1FFFF);
          }
          else {
            showind++;
            if (showind<=ind2) showind=ind1;
            if (showind>ind1) showind=ind2+1;
            //get value of first byte, then cycle thru each byte and compare
            //should be all equal values in block for this demo...
            //  *IF* user used calloc or pressed 'ON' to fill block with value
            //  If user used only malloc and did not use 'ON' to fill block, then random
            //    different values would be most likely found.
            change = 0;
            val = *(ptr[showind]);
            for (i=0; i < size[showind]; i++) {
                if ( *(ptr[showind] + i) != val ) change = 1;
            }
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
            if (change==0) {
                sprintf(str,"Block #%d @ %x, size %d: values are all the same, =%d",\
                    showind, ptr[showind], size[showind], val);
            }
            else {
                sprintf(str,"Block #%d @ %x, size %d: values are varying!",\
                    showind, ptr[showind], size[showind]);
            }
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
            Delay(0x1FFFF);
          }
        }
        if (key & BUTTONS_AV300_ON) {//   ON button, fill with numbers
            for (i=0; i < size[ind1]; i++)
                *(ptr[ind1] + i) = ind1; //fills each byte of block with the block #
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,strblank);
            sprintf(str,"Filled block #%d:  %d bytes, with value '%d'.",\
                ind1, size[ind1], ind1);
            graphicsStringA(&screenBitmap,0,50,&font5x8,std5x8_,5,0,str);
            Delay(0x1FFFF);
        }
        if (key & BUTTONS_AV300_OFF) {//  OFF button, exit demo 
            return;
        }
    }
}


//----routine to provide Delay-----
void Delay(unsigned int time) {
    for (;time>0;time--) ;
}

