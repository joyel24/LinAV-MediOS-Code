#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <ata.h>

void delay(unsigned int time);
void doScroll(int xdir, int ydir);

int scroll = 0;
int slow = 1;
int wait = 0x01200;
int xv = 0, yv = 0, xb = 60, yb = 100;

//make two-color pallettes for text on video (pal32) and bitmap (pal16) planes.
//First number is background color, second is text color.
static int pal32[2] = {0x80800080, 0x8080ff80}; //CrCrYCb order, 32-bit color: {black,white}
static int pal16[2] = {0x0101, 0xffff}; //indexes to the 256-color pallette: {red,white}

//make structures for each video component/plane we'll be using (three of them)
struct graphicsBuffer screenVideo;
struct graphicsBuffer screenVideo2;
struct graphicsBuffer screenBitmap;

//make and init structures for each font we'll be using
//spriteFont5x8 will be filled manually in main(), the other three use short method
struct graphicsBuffer spriteFont5x8;
struct graphicsBuffer spriteFont7x13 =  {0,1,7,13,1,0,-1,0,0,0,0,(int**)&pal16,(int**)&pal32};
struct graphicsBuffer spriteFontCursive={0,2,9,15,1,0, 0,0,0,0,0,(int**)&pal16,(int**)&pal32};


int main() {

// Might as well power down HD and save battery :)

    ataPowerDownHDDA();

//-----------------INIT spriteFont5x8 STRUCTURE  ----------------------------
    //long way of initializing font structure spriteFont5x8:
    spriteFont5x8.offset = 0;               //no need to use offset for font sprites
    spriteFont5x8.bytesPerLine = 1;	        //# of bytes horizontally in font source per char.
    spriteFont5x8.width = 5;                //actual width of one character in pixels
    spriteFont5x8.height = 8;               //actual height in pixels
    spriteFont5x8.bitsPerPixel = 1;         //each pixel in font source uses 1 bit (2-color)
    spriteFont5x8.bitsPerPixelShift = 0;    //use 0 since this is only 2-color sprite
    spriteFont5x8.transparent = 0;          //color 0 index in font source shall be transparent
	                                        //-1=no transparancy, 1 would print inverse text?
    spriteFont5x8.pallette1 = 0;            //not necessary for font sprites
    spriteFont5x8.pallette2 = 0;            //ditto
    spriteFont5x8.pallette4 = 0;            //ditto
    spriteFont5x8.pallette8 = 0;            //ditto
    spriteFont5x8.pallette16 = (int**)&pal16;//point to our pal16 for printing on bitmap
    spriteFont5x8.pallette32 = (int**)&pal32;//point to our pal32 for printing on video

    //the short way of initializing font structures shown earlier, before main(), simply
    //  corresponds to this long way, each member is in order as shown here.
    //Note that the spriteFontCursive has '2' as bytesPerLine, cause it's 9pix wide that means
    //  the source uses 2 bytes for each line of each character.
    //Also note spriteFont7x13 has transparent member set to -1 (I don't want transparancy)


//-----------------INIT screenVideo AND screenBitmap STRUCTURES -------------

    //screenVideo will be size of LCD only
    screenVideo.offset = 0x03c00000;    //use typical address for video1 memory
    screenVideo.bytesPerLine = 320*4;   //320pixels wide * 4bytes (32-bits) per pixel
    screenVideo.width = 320;            //width 320pix
    screenVideo.height = 240;           //height 240pix
    screenVideo.bitsPerPixelShift = 5;  //use '5' for 32-bit graphics here
    screenVideo.bitsPerPixel = 32;      //must be 32 for video planes

    //screenVideo2 will be 640x480 for scrolling example
    screenVideo2.offset = 0x03e00000;   //use typical address for video2 memory
    screenVideo2.bytesPerLine = 640*4;  //640pix wide * 4bytes per pixel
    screenVideo2.width = 640;           //width 640pix!
    screenVideo2.height = 480;          //height 480pix!
    screenVideo2.bitsPerPixelShift = 5; //use '5' for 32-bit graphics
    screenVideo2.bitsPerPixel = 32;     //must use 32 for video planes
    
    //bitmap1 will be only 200x100 used for displaying some text info, don't need whole screen
    screenBitmap.offset = 0x03800000;   //use typical address for bitmap1 memory
    screenBitmap.bytesPerLine = 208*2;  //208pix wide * 2bytes (16-bits) per pixel
      //Why 208 pix?  because the 200*2 wouldn't be a multiple of 32, the min boundary
      //   for scrolling purposes and offset changes.
      //   So, we use the next larger size divisible by 32 for better memory organization.
    screenBitmap.width = 200;           //width 200pix
    screenBitmap.height = 100;          //height 100pix
    screenBitmap.bitsPerPixelShift = 4; //use '4' for 16-bit graphics
    screenBitmap.bitsPerPixel = 16;     //must use 16 for bitmap planes



//-----------------INIT OSD SYSTEM ------------------------------------------

    //call osdInit and disable everything just to be safe
    osdInitA();
    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);
    //set up VIDEO1 as needed; will correspond to our screenVideo structure
    osdSetComponentSizeA(OSD_VIDEO1, 640, 240);
            //640=twice desired window width, 240=desired window height
            //you always use twice desired width, even for bitmap components
    osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
            //the typical position that sets this component at 0,0 of LCD
            //these numbers seem to get all components starting correctly at upper left of
            //the LCD, so use these as a basis.  Adding 2 to the x coordinate (0x14 now)
            //would shift display 1 pixel to right, adding 1 to y coordinate would shift
            //display 1 pixel down.  This applies to both video or bitmap components.
            //This setting can also be used later to scroll a window that's smaller than LCD.
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03c00000);
            //point to our screenVideo memory, this is a setting that can later be used in
            //scrolling a window that's larger than the LCD.
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
            //set to screenVideo.bytesPerLine divided by 32 (320*4/32 = 40 = 0x28)
            //note this is Source memory area width, not lcd or window width.  This is why
            //earlier we had to keep the BytesPerLine setting at multiples of 32...
    osdSetComponentConfigA(OSD_VIDEO1, 0);
            //keep the video1 component hidden for now, won't show on screen yet.

    //set up VIDEO2 as needed; will correspond to our screenVideo2 structure
    osdSetComponentSizeA(OSD_VIDEO2, 640, 240);
            //640=twice desired window width, 240=desired window height
    osdSetComponentPositionA(OSD_VIDEO2, 0x14, 0x12);
            //the typical position that sets this component at 0,0 of LCD
    osdSetComponentOffsetA(OSD_VIDEO2, 0x03e00000);
            //point to our screenVideo2 memory -- we'll alter this later for scrolling
            //  So for now it's showing the default top left of 640x480 video memory
            //Since this window is bigger than display, you could initialize it here to a 
            //  different default location...say you want LCD to show the middle lower
            //  area initially:  desired top left x,y would be something like 160,240 so
            //  you'd use 0x03e00000 + (y*4*640) + (x*4) or: 0x03e00000+(240*4*640)+(160*4).
            //  The '4' is number bytes per pixel, so you'd use '2' for bitmap planes.
            //  The '640' is number of pixels per line in the source memory
    osdSetComponentSourceWidthA(OSD_VIDEO2, 0x50);
            //set to screenVideo2.bytesPerLine divided by 32 (640*4/32 = 80 = 0x50)
    osdSetComponentConfigA(OSD_VIDEO2, 0);
            //keep it disabled for now, we'll switch between the two video planes later.

    //set up BITMAP1 as needed; will correspond to our screenBitmap structure
    osdSetComponentSizeA(OSD_BITMAP1, 400, 100);
            //400=twice desired window width, 100=desired window height
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
            //the typical position that sets this component at 0,0 of LCD
            //this will start this window at 0,0 location (upper left).  Since this window
            //  is smaller than screen, we'd use this setting if we want to start it at a
            //  different spot on the screen.  Here it's always x*2 + 0x14, y+0x12.
            //  So if you want upper left of window at 50,70 you'd use (50*2 + 0x14, 70+0x12)
            //  THis formula does not change for video planes, use the same.
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
            //point to our screenBitmap memory
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x0D);
            //set to screenBitmap.bytesPerLine divided by 32 (208*2/32 = 13 = 0x0D)
    osdSetComponentConfigA(OSD_BITMAP1, 0);
            //keep it disabled until later


//------------------------ DRAW SOME STUFF ------------------------------------------

    int x,y,c;

    //draw a green border 4 pix wide around the 200x100 screenBitmap layer (BITMAP1)
    graphicsBoxfA(&screenBitmap, 0, 0, 4, 100, 0xfafa); //green is 0xfa on built-in pallette
        //starts at 0,0 and draw 4 wide 100 high in green bitmap pallette color (left edge)
    graphicsBoxfA(&screenBitmap, 196, 0, 4, 100, 0xfafa); //(right edge)
    graphicsBoxfA(&screenBitmap, 0, 0, 200, 4, 0xfafa);  // (top edge)
    graphicsBoxfA(&screenBitmap, 0, 96, 200, 4, 0xfafa);  //(bottom edge)
    //fill center area with dark gray background
    graphicsBoxfA(&screenBitmap, 4,4,192,92,0x1212); //gray is 0x12 on built-in pallette
    //reposition box so that it's more in center of screen
    osdSetComponentPositionA(OSD_BITMAP1, 0x14 + (60*2), 0x12 + 100);
              //0x14 and 0x12 are equivalent to 0,0 position on LCD
              //we want it at 60,100 location:  x is doubled and added to 0x14 (x*2 is always
              //   used here for both video and bitmap layers).  y is just added to 0x12.

    //draw the words 'Please Wait!' on bitmap1, cause rest of drawing takes a while...
    //  This command graphicsString is explained in the next section of program.
    graphicsStringA(&screenBitmap, 43, 8, &spriteFontCursive, cursive_, 9, 0, "Please Wait!");

    //enable bitmap1 component
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A6);
         //show it on screen.  don't know OSD_BITMAP_8BIT ? don't know OSD_BITMAP_MERGEBACK ?
         //OSD_BITMAP_A6 sets blending, to make video layer show thru bitmap just a little.
         //   OSD_BITMAP_A0 will cause bitmap to disappear cause lower layers will cover it.
         //   OSD_BITMAP_A7 will make bitmap 100% visible, layers under will be invisible.
         //   Inbetween values cause varying amounts of opacity

    //draw a blue border 2pix wide around edge of the 320x240 screenVideo layer (VIDEO1)
    graphicsBoxfA(&screenVideo, 0, 0, 2, 240, 0x404050FF);
        //start at 0,0 and draw 2 wide 240 high in blue (CbCbYCr order 32-bit) (left border)
    graphicsBoxfA(&screenVideo, 318, 0, 2, 240, 0x404050FF); //(right border)
    graphicsBoxfA(&screenVideo, 0, 0, 320, 2, 0x404050FF); // (top border)
    graphicsBoxfA(&screenVideo, 0, 238, 320, 2, 0x404050FF); // (bottom border)
    //now fill center of screenVideo with various colors
    for (x=2; x<318; x++) {
        for (y=2; y<238; y++) {
            c = 0xC0C04000 + ((x+y)<<8);   //just generate some various colors
            //draw one pixel at a time in x,y location with color c
            graphicsSetPixelA(&screenVideo, x, y, c);
        }
    }

    //draw a red border 2pix wide around edge of the 640x480 screenVideo2 layer (VIDEO2)
    graphicsBoxfA(&screenVideo2, 0, 0, 2, 480, 0xFFFF5050);
    graphicsBoxfA(&screenVideo2, 638, 0, 2, 480, 0xFFFF5050);
    graphicsBoxfA(&screenVideo2, 0, 0, 640, 2, 0xFFFF5050);
    graphicsBoxfA(&screenVideo2, 0, 478, 640, 2, 0xFFFF5050);
    //fill center of screenVideo2 with various colors/pattern
    for (x=2; x<638; x++) {
        for (y=2; y<478; y++) {
            c = 0xf0f0f0f0 - ((x*y) << 9);  //another random formula, turns out it looks cool
            //draw one pixel at a time in x,y location with color c
            graphicsSetPixelA(&screenVideo2, x, y, c);
        }
    }

    //enable video1 component, we'll show that first
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);


//------------------------ PRINT INITIAL TEXT IN VARIOUS PLACES ---------------------
    
    //clear the area that used to say "Please Wait!":
    graphicsBoxfA(&screenBitmap, 10, 8, 180, 15, 0x1212); //draws box in gray
    
    //change to text color red for next textline:
    //(adjusts pal16 because we're printing on 16-bit graphics)
    //(adjusts color index 1 because that's the text color; index 0 is the background color)
    pal16[1] = 0xF9F9;  //0xf9 is red in the built-in 256-color pallette

    //Next statement prints string to screen.  The arguments are in this order:
    //1. Pointer to structure of video component to print to
    //2. x-location of upper left start
    //3. y-location of upper left start
    //4. Pointer to structure of font sprite to use
    //5. Pointer to actual font data (from the various fonts available in fonts.h)
    //6. horizontal amount of pixels to advance with each character...see below
    //7. vertical amount of pixels to advance with each character...see below
    //8. The text string (pointer to char array)
    //Arguments 6 and 7 allow you to print in different directions or spacings.
    //Typically you'd set horizontal advance to pixel width of one char (spriteFont5x8.width)
    //  so that each char takes minimum amount of space.  However, using a larger number
    //  provides a perfect way of spacing characters out to fill a larger area.
    //Typically you'd set vertical advance to 0, for horizontal text (no vertical advance).
    //  But, using higher than zero makes diagonal text, each character lower on screen.
    //Using hor. advance of zero with vert. advance equal or higher than real character height
    //  makes text print vertically!
    graphicsStringA(&screenBitmap, 43, 8, &spriteFontCursive, cursive_, 9, 0, "Graphics Test");

    //change to yellow for next line
    pal16[1] = 0xFBFB;
    graphicsStringA(&screenBitmap, 10, 26, &spriteFont5x8, std5x8_, 5, 0,
                  "This is Bitmap1 Component, 256 Color");
    //change back to white
    pal16[1] = 0xFFFF;
    //next line will print not transparent (that's how we defaulted spriteFont7x13)
    //with red as background color since we're using pal16 (printing to 16-bit graphics)
    //and the zero-index (text background) color was defined as red in initizalization
    //also its characters will be more spaced out, I used 10 for horizontal advance and the
    //actual character width is only 7.  Notice how it looks on screen.  If it were
    //also transparent, the spacing out wouldn't be so noticeable and would just look nice.
    graphicsStringA(&screenBitmap, 12, 40, &spriteFont7x13, std7x13_, 10, 0,
                  "Controls/Settings:");

    graphicsStringA(&screenBitmap, 15, 58, &spriteFont5x8, std5x8_, 5, 0,
                  "F1 - DISPLAY  [now: Video1]");
    graphicsStringA(&screenBitmap, 15, 68, &spriteFont5x8, std5x8_, 5, 0,
                  "F2 - SCROLL   [now: Bitmap1]");
    graphicsStringA(&screenBitmap, 15, 77, &spriteFont5x8, std5x8_, 5, 0,
                  "   (use joystick to scroll)");
    graphicsStringA(&screenBitmap, 15, 86, &spriteFont5x8, std5x8_, 5, 0,
                  "F3 - SPEED    [now:  Medium]");

    //Few lines earlier we used the spriteFont7x13 on bitmap -- it produced white text with
    //no transpareny and red background.  We use it here again, with no changes, but it'll
    //produce white text with black background.  It's because we're now using it on the
    //video components, which means it looks at pal32 structure instead of pal16 to decide
    //what colors to use...
    graphicsStringA(&screenVideo, 10, 20, &spriteFont7x13, std7x13_, 7, 0,
                  "This is the Video1 Component.  It's 320x240");
    graphicsStringA(&screenVideo2, 10, 20, &spriteFont7x13, std7x13_, 7, 0,
                  "This is the Video2 Component.  It's sized 640x480 and scrollable in any direction.");

    //change to transparent mode for next lines and change color to red
    spriteFont7x13.transparent = 0;
    //alter pal32 this time cause we're printing on 32-bit video now.
    pal32[1] = 0xFFFF4050;  //red in CrCrYCb order
    graphicsStringA(&screenVideo, 40, 50, &spriteFont7x13, std7x13_, 7, 0,
                  "This Component can not be scrolled");
    //change to red
    pal32[1] = 0x9090FF00;
    graphicsStringA(&screenVideo2, 10, 50, &spriteFont7x13, std7x13_, 7, 0,
                  "Component can be scrolled normal or smooth");

    //change color to purple for this line:
    pal32[1] = 0x90905090;
    //and set transparancy back to off but instead of black background use yellow:
    spriteFont7x13.transparent = -1;
    pal32[0] = 0x9090FF00;  //color index 0 sets text background color
    graphicsStringA(&screenVideo2, 10, 250, &spriteFont7x13, std7x13_, 7, 0,
                  "Difference in normal & smooth horizontal scroll is obvious at medium or slow speeds...");

    //set 5x8 font to what I need for rest of program
    pal16[0] = 0x1212;  //gray background
    spriteFont5x8.transparent = -1;  //not transparent so I can overwrite existing text
                                     //without having to "erase" is first...

//------------------------ MAIN LOOP FOR DEMO ---------------------------------------
    int key;
    int layer = 0; 
    while (1) {
        key = buttonsGetStatusA();
        if (key & BUTTONS_AV300_MENU1) {    //F1 button pressed, switch displayed video layer
            layer = !layer;
            if (layer==0) {  //enable video1, disable video2
                osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);
                osdSetComponentConfigA(OSD_VIDEO2, 0);
                //update the text to reflect current mode!
                graphicsStringA(&screenBitmap, 15, 58, &spriteFont5x8, std5x8_, 5, 0,
                  "F1 - DISPLAY  [now: Video1]");

            }
            else {  //enable video2, disable video1
                osdSetComponentConfigA(OSD_VIDEO2, OSD_COMPONENT_ENABLE);
                osdSetComponentConfigA(OSD_VIDEO1, 0);
                graphicsStringA(&screenBitmap, 15, 58, &spriteFont5x8, std5x8_, 5, 0,
                  "F1 - DISPLAY  [now: Video2]");
            }
            delay(0x10000);  //delay to avoid multiple keypresses too fast
        }
        if (key & BUTTONS_AV300_MENU2) {  //F2 button pressed, toggle scroll control
            scroll++; if (scroll>2) scroll=0;
            if (scroll==0) { //joystick controls will move bitmap layer
                graphicsStringA(&screenBitmap, 15, 68, &spriteFont5x8, std5x8_, 5, 0,
                  "F2 - SCROLL   [now: Bitmap1]      ");
            }
            else if (scroll==1) { //joystick controls will move video2 (w/coarse horizontal)
                //reset stuff that was changed for the smooth horizontal scrolling mode...
                osdSetComponentSizeA(OSD_VIDEO2, 640, 240);
                osdSetComponentPositionA(OSD_VIDEO2, 0x14, 0x12);
                graphicsStringA(&screenBitmap, 15, 68, &spriteFont5x8, std5x8_, 5, 0,
                  "F2 - SCROLL   [now: Video2 Normal]");
            }
            else {  //joystick controls will move video2 (w/smooth horizontal scrolling)
                //do trick to facilitate smooth horizontal scrolling (see "doScroll" routine)
                osdSetComponentSizeA(OSD_VIDEO2, 656, 240);
                graphicsStringA(&screenBitmap, 15, 68, &spriteFont5x8, std5x8_, 5, 0,
                  "F2 - SCROLL   [now: Video2 Smooth]");
            }
            delay(0x10000);
        }
        if (key & BUTTONS_AV300_MENU3) {  //F3 button pressed, switch between slow/fast scroll
            slow++; if (slow>2) slow=0;
            if (slow==0) {
                wait = 0x6000;
                graphicsStringA(&screenBitmap, 15, 86, &spriteFont5x8, std5x8_, 5, 0,
                  "F3 - SPEED    [now:  Slow]  ");
            }
            else if (slow==1) {
                wait = 0x1200;
                graphicsStringA(&screenBitmap, 15, 86, &spriteFont5x8, std5x8_, 5, 0,
                  "F3 - SPEED    [now:  Medium]");
            }
            else {
                wait = 0x0300;
                graphicsStringA(&screenBitmap, 15, 86, &spriteFont5x8, std5x8_, 5, 0,
                  "F3 - SPEED    [now:  Fast]  ");
            }
            delay(0x10000);
        }
        if (key & BUTTONS_AV300_UP) doScroll(0,-1);
        if (key & BUTTONS_AV300_RIGHT) doScroll(1,0);
        if (key & BUTTONS_AV300_DOWN) doScroll(0,1);
        if (key & BUTTONS_AV300_LEFT) doScroll(-1,0);
        if (key & BUTTONS_AV300_OFF) return 0;
    }
}


//------------------------ ROUTINE TO SCROLL ----------------------------------------

void doScroll(int xdir, int ydir) {

    int pixshift;
    //First, handle scrolling of bitmap component...
    //This is easy, since bitmap window is smaller than screen, we just use the
    //   osdSetComponentPosition to place it in new spot.
    //This implements fine pixel-by-pixel scrolling easily.
    if (scroll==0) {
        xb += xdir; yb += ydir;  //move in proper direction
        //keep coordinates within range so whole bitmap component always stays on screen,
        //   since we want to keep the text inside readable and on-screen.
        if (xb<0) xb=0; if (yb<0) yb = 0;
        if (xb>120) xb=120; if (yb>140) yb = 140;
        //Set the new position -- as before, we double x direction, and we
        //  add the default top left position of 0x14,0x12 to both x and y:
        osdSetComponentPositionA(OSD_BITMAP1, 0x14 + (xb*2), 0x12 + yb);
        //use delay to avoid multiple keypresses being registered and scrolling too fast:
        delay(wait);
    }

    //OK, now we handle scrolling of the video2 component...
    //Since video2 is larger than screen, we can't use osdSetComponentPosition.
    //We have to use the osdSetComponentOffset control, so that we "move" through video memory
    //  and mimic scrolling that way.
    //Since offset is limited to nearest 32-byte boundary, we can only scroll in increments
    //  of eight pixels ONLY in a horizontal direction.  Vertical is fine, pixel-by-pixel
    else if (scroll==1) {
        xv += xdir; yv += ydir;
        //keep from scrolling too much out of boundary...always want to keep lcd full
        if (xv<0) xv = 0; if (yv<0) yv = 0;
        if (xv>320) xv = 320; if (yv>240) yv = 240;
        //here we set the offset:  base address for video2, PLUS
        //    y_position * bytes_per_pixel * width_in_pixels, PLUS x_position * bytes_per_pix
        //In this case it's video component, so 32-bit, means 4 bytes per pixel
        //    and width of video memory is 640 pixels across:
        //Note that this formula still sets offset at numbers other than 32-byte boundaries --
        //    That's OK, av300 just ignores it -- but the result is that the scroll will only
        //    show when this routine gets run 8 times (scrolling by 8pixels, which then has 
        //    advanced the offset to the next 32-byte boundary.
        //    I left it this way even though it seems useless to have to let it run 8 times
        //    before it actually scrolls -- the reason is that if I made it actually jump
        //    8 pixels (32bytes) every time this loop was run, then in use the horizontal
        //    scrolling would appear to move 8 times faster than vertical, and it'd be a mess
        //    to adjust the delay to fix it...
        osdSetComponentOffsetA(OSD_VIDEO2, 0x03e00000 + (yv * 4 * 640) + (xv * 4));
        //delay:
        delay(wait);
    }

    //Last, we handle the scrolling of video2 using a special trick to implement smooth
    //   horizontal scrolling, not limited by the 8-pixel limitation of above method.
    else {
        xv += xdir; yv += ydir;
        if (xv<0) xv = 0; if (yv<0) yv = 0;
        if (xv>320) xv = 320; if (yv>240) yv = 240;
        //First we use offset just like above.  It'll display to nearest 32-byte boundary,
        //   meaning nearest 8pixels horizontally
		osdSetComponentOffsetA(OSD_VIDEO2, 0x03e00000 + (yv * 4 * 640) + (xv * 4));
        //Now calculate how many pixels past nearest 8pix boundary for this desired position
		pixshift = xv % 8;
        //Now we use osdSetComponentPosition, which allows pixel-level positioning, in order
        //to shift entire display slightly (we need up to 7pix) in order to provide the smooth
        //scrolling we're looking for.  As in other uses of osdSetComponentPosition,
        //we must use double the x position (pixshift*2) and in this case we subtract it from
        //the default 0x14 (which was equal to 0 position at left edge of LCD).  We use the
        //default 0x12 for vertical since it doesn't need alteration.
		osdSetComponentPositionA(OSD_VIDEO2, 0x14 - (pixshift * 2), 0x12);
        //One important thing:  doing this position shifting, where we're moving display to
        //left by a few pixels, will cause the right edge of the display to show black or
        //garbage, because the OSD for video2 layer is only 320pixels wide, and we're shifting
        //the left-most pixels off the screen, leaving the right edge blank.
        //The fix was done back in main() main loop, we did the following for smooth scroll:
        //osdSetComponentSize(OSD_VIDEO2, 656, 240);
        //This tricks the OSD into making Video2 display 328 pixels on LCD
        //  (328*2=656, ComponentSize is always desired pixels * 2 for x direction).
        //  The extra 8 pixels means the video is drawn wider than the LCD can display
        //  which is exactly what we need for smooth scrolling.  Then we can shift as much as
        //  7 pixels to the left and not expose garbage on the right edge of display...!
        delay(wait);
    }
}


//----routine to provide delay-----
void delay(unsigned int time) {
    for (;time>0;time--) ;
}
