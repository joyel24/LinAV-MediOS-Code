#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <ata.h>

#include <file.h>

void delay(unsigned int time);
void doScroll(int xdir, int ydir);
int colorRGB2Packed(int r, int g, int b);

int scroll = 2;
int slow = 1;
int wait = 0x01200;
int xv = 0, yv = 0, xb = 60, yb = 100;



void initTextReader(char * filename);
void updateScreen();

void runMainLoop();

int screen_y_req = 0;


//#define SHOULD_KEEP_ALL_LINEFEEDS

// 7x13
//#define	VISIBLE_LINES		19
//#define AREA_COLUMNS		44
//#define FONTSIZE_X			7
//#define FONTSIZE_Y			13

// 5x8
#define	VISIBLE_LINES		30
#define AREA_COLUMNS		62
#define FONTSIZE_X			5
#define FONTSIZE_Y			8



#define	AREA_LINES			(2*VISIBLE_LINES)

#define COLOR_BLACK			0x80800080
#define COLOR_WHITE			0x8080ff80

#define COLOR_BACKGROUND		COLOR_WHITE
#define COLOR_FOREGROUND		COLOR_BLACK


struct ColorSet {
	int background;
	int foreground;
	int slider;
	int sliderEdge;
	int bottom;
};

struct ColorSet defaultColorSets[] = {	 { COLOR_WHITE, COLOR_BLACK, 0x0909, 0x1010, 0x0909 }
										,{ COLOR_BLACK, COLOR_WHITE, 0x5656, 0x0A0A, 0x5656 }
										,{  0x8080d080, COLOR_BLACK, 0x0909, 0x1010, 0x0909 }
										,{ COLOR_BLACK, 0, 0x7676, 0x7575, 0x7676 }
										,{ COLOR_WHITE, COLOR_BLACK, 0x0707, 0x0A0A, 0x0808 }
										,{ COLOR_BLACK, 0, 0x3636, 0x0A0A, 0x3636 }
										};

void initTextArea();

//#include "title_areader_yuv.h"

int activeColorSetNr = 0;
int defaultColorSetCount = sizeof(defaultColorSets) / sizeof(struct ColorSet);

struct ColorSet activeColorSet;

//make two-color pallettes for text on video (pal32) and bitmap (pal16) planes.
//First number is background color, second is text color.
static int pal32[2] = {COLOR_WHITE, COLOR_BLACK}; //CrCrYCb order, 32-bit color
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


struct graphicsBuffer* activeFont = &spriteFont5x8;
char** activeFont_ = std5x8_;
//struct graphicsBuffer* activeFont = &spriteFont7x13;
//char** activeFont_ = std7x13_;


int timeToQuit = 0;

int main(int argc, char * * argv) {
	defaultColorSets[3].foreground = colorRGB2Packed(0,255,0);
	defaultColorSets[5].foreground = colorRGB2Packed(255,90,0);

//-----------------INIT spriteFont5x8 STRUCTURE  ----------------------------
    //long way of initializing font structure spriteFont5x8:
    spriteFont5x8.offset = 0;               //no need to use offset for font sprites
    spriteFont5x8.bytesPerLine = 1;	        //# of bytes horizontally in font source per char.
    spriteFont5x8.width = 5;                //actual width of one character in pixels
    spriteFont5x8.height = 8;               //actual height in pixels
    spriteFont5x8.bitsPerPixel = 1;         //each pixel in font source uses 1 bit (2-color)
    spriteFont5x8.bitsPerPixelShift = 0;    //use 0 since this is only 2-color sprite
    spriteFont5x8.transparent = -1;          //color 0 index in font source shall be transparent
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

    //screenVideo2 will be 320x480 for scrolling example
    screenVideo2.offset = 0x03e00000;   //use typical address for video2 memory
    screenVideo2.bytesPerLine = 320*4;  //640pix wide * 4bytes per pixel
    screenVideo2.width = 320;           //width 320pix!
//    screenVideo2.height = 480;          //height 480pix!
    screenVideo2.height = 500;          //height 500pix!
    screenVideo2.bitsPerPixelShift = 5; //use '5' for 32-bit graphics
    screenVideo2.bitsPerPixel = 32;     //must use 32 for video planes
    
    //bitmap1 will be only 200x100 used for displaying some text info, don't need whole screen
    screenBitmap.offset = 0x03800000;   //use typical address for bitmap1 memory
    screenBitmap.bytesPerLine = 320*2;  //320 pix wide * 2bytes (16-bits) per pixel
    screenBitmap.width = 320;           //width 320pix
    screenBitmap.height = 240;          //height 240pix
    screenBitmap.bitsPerPixelShift = 4; //use '4' for 16-bit graphics
    screenBitmap.bitsPerPixel = 16;     //must use 16 for bitmap planes



//-----------------INIT OSD SYSTEM ------------------------------------------

    //call osdInitA and disable everything just to be safe
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
    osdSetComponentSourceWidthA(OSD_VIDEO2, 0x28);
            //set to screenVideo2.bytesPerLine divided by 32 (640*4/32 = 80 = 0x50)
    osdSetComponentConfigA(OSD_VIDEO2, 0);
            //keep it disabled for now, we'll switch between the two video planes later.

    //set up BITMAP1 as needed; will correspond to our screenBitmap structure
    osdSetComponentSizeA(OSD_BITMAP1, 640, 240);
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
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
            //set to screenBitmap.bytesPerLine divided by 32 (320*2/32 = 20 = 0x14)
    osdSetComponentConfigA(OSD_BITMAP1, 0);
            //keep it disabled until later


    int x,y,c;

/*
//	draw the intro screen while the display is off
//  (incredibly inefficient probably, but works in OK time)
	int bin_data_offset = 0;
	int* scrdata = (int*) screenVideo2.offset;
	int* thetitle = (int*) titlepic_yuv;
    for (y=0; y<240; y++) {
		for (x=0; x<320; x++) {
			*scrdata++ = *thetitle++;

            //draw one pixel at a time in x,y location with color c
//            graphicsSetPixel(&screenVideo2, x, y, colorRGB2Packed(bin_data[bin_data_offset], bin_data[bin_data_offset+1], bin_data[bin_data_offset+2]));

			bin_data_offset += 3;
        }
    }*/

//------------------------ DRAW SOME STUFF ------------------------------------------

	timeToQuit = 0;

	osdSetComponentConfigA(OSD_VIDEO2, OSD_COMPONENT_ENABLE);

	initTextReader(argv[1]);

	int key;
	//while (!((key = buttonsGetStatus()) & BUTTONS_AV300_ANY));

	activeColorSetNr = 0;
	while (timeToQuit == 0) {
		activeColorSet = defaultColorSets[activeColorSetNr];
		pal32[0] = activeColorSet.background;
		pal32[1] = activeColorSet.foreground;

//------------------------ MAIN LOOP FOR DEMO ---------------------------------------
		initTextArea();

		runMainLoop();

		osdSetComponentConfigA(OSD_VIDEO1, 0);
		osdSetComponentConfigA(OSD_VIDEO2, 0);
		osdSetComponentConfigA(OSD_BITMAP1, 0);
		osdSetComponentConfigA(OSD_BITMAP2, 0);
		osdSetComponentConfigA(OSD_CURSOR1, 0);
		osdSetComponentConfigA(OSD_CURSOR2, 0);

		activeColorSetNr = (activeColorSetNr+1) % defaultColorSetCount;
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
        if (xv<0) xv = 0;
        if (xv>320) xv = 320;

		xv = 0;		// no horiz scrolling

		while (yv<0) yv += (VISIBLE_LINES*FONTSIZE_Y);
		while (yv>=(VISIBLE_LINES*FONTSIZE_Y)) yv -= (VISIBLE_LINES*FONTSIZE_Y);			// scroll wrap!
//		if (yv>260) yv = 260;
//		if (yv>240) yv = 240;
        //First we use offset just like above.  It'll display to nearest 32-byte boundary,
        //   meaning nearest 8pixels horizontally
		osdSetComponentOffsetA(OSD_VIDEO2, 0x03e00000 + (yv * 4 * 320) + (xv * 4));
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
    }
}


//----routine to provide delay-----
void delay(unsigned int time) {
    for (;time>0;time--) ;
}


int nrOfCharsToDrawPerUpdate	= 45;

int lastIndicatorDrawnPos;
int lastIndicatorDrawnSize;

int showTopLine = 0;

int	drawChars = 1;
int reqLine[AREA_LINES];
int actualLine[AREA_LINES];
int remaining[AREA_LINES];
int nrOfLines = 0;

//#include "BookRev.h"
//#include "KingLear.h"

char textFile[400000] = "This is an extremely small text file example.\n\nBlah blah blah.\n...\n";

//#include "TheRaven.h"

char processedText[500000];
char* linePtr[50000];

int getWordLength(char* txt) {
	int len = 0;
	while ((*txt != 0) && (*txt != '\n') && (*txt != '\r') && (*txt != '\t') && (*txt != ' ')) {
		len++;
		txt++;
	}
	return len;
}
int skipWhitespaceLength(char* txt, int* lineCount) {
	int len = 0;
	*lineCount = 0;
	while ((*txt != 0) && ((*txt == '\n') || (*txt == '\r') || (*txt == '\t') || (*txt == ' '))) {

		if (*txt == '\n') {
			*lineCount = 1 + *lineCount;
		}
		len++;
		txt++;
	}
	return len;
}
void copyN(char* src, char* dest, int N) {
	while (N > 0) {
		*dest++ = *src++;
		N--;
	}
}
int lineLen(char* linePtr) {
	int len = 0;
	while (*linePtr != '\n')  {
		len++;
		linePtr++;
	}
	return len;
}


void printNr(char* buffer, int nr) {
	buffer[0] = '0' + (nr / 100) % 10;
	buffer[1] = '0' + (nr / 10) % 10;
	buffer[2] = '0' + (nr / 1) % 10;
	buffer[3] = 0;
}


void updateLine(int nr, int pos);
void updateScreen(int startAtLine) {
	drawChars = nrOfCharsToDrawPerUpdate;
	char lineNrBuffer[10];
	int i_a;
	int i;

	for (i_a=0; i_a<AREA_LINES; i_a++) {
		i = (startAtLine + i_a) % AREA_LINES;
		if (reqLine[i] != actualLine[i]) {
			actualLine[i] = reqLine[i];
			remaining[i] = lineLen(linePtr[reqLine[i]]);
		    graphicsBoxfA(&screenVideo2, 0, i*FONTSIZE_Y, AREA_COLUMNS*FONTSIZE_X+1, FONTSIZE_Y, activeColorSet.background);
		}
	}
	for (i_a=0; i_a<AREA_LINES; i_a++) {
		i = (startAtLine + i_a) % AREA_LINES;

		printNr(lineNrBuffer, i);
//		graphicsStringA(&screenVideo2, 200, i*FONTSIZE_Y, &spriteFont7x13, std7x13_, 7, 0, lineNrBuffer);
		printNr(lineNrBuffer, actualLine[i]);
//		graphicsStringA(&screenVideo2, 250, i*FONTSIZE_Y, &spriteFont7x13, std7x13_, 7, 0, lineNrBuffer);
		printNr(lineNrBuffer, reqLine[i]);
//		graphicsStringA(&screenVideo2, 300, i*FONTSIZE_Y, &spriteFont7x13, std7x13_, 7, 0, lineNrBuffer);

		if (reqLine[i] != actualLine[i]) {
			// should never get here?!
		} else if (remaining[i] > 0) {
			updateLine(actualLine[i], i);
		}

		if (drawChars <= 0) {
			break;
		}
	}

	// draw size indicator

	int indSize = (VISIBLE_LINES-1)*FONTSIZE_Y*VISIBLE_LINES / nrOfLines;
	if (indSize > ((VISIBLE_LINES-1)*FONTSIZE_Y)) {
		indSize = (VISIBLE_LINES-1)*FONTSIZE_Y;
	}
	if (indSize < 5) {
		indSize = 5;
	}
	int indPos;
	int maxShowTop = nrOfLines - VISIBLE_LINES;
	if (maxShowTop > 0) {
		indPos = ((VISIBLE_LINES-1)*FONTSIZE_Y - indSize)*showTopLine;
		indPos = indPos / maxShowTop;
	} else {
		indPos = 0;
	}

	if ((lastIndicatorDrawnSize != indSize) || (lastIndicatorDrawnPos != indPos)) {
		int scrSize = (VISIBLE_LINES-1)*FONTSIZE_Y;

		if (indPos>0) {
			graphicsBoxfA(&screenBitmap, 306, 0, 13, indPos, 0x0000);
		}
		if ((indPos+indSize) < scrSize) {
			graphicsBoxfA(&screenBitmap, 306, indPos+indSize, 13, scrSize - (indPos+indSize), 0x0000);
		}

//		graphicsBoxfA(&screenBitmap, 307,    indPos,         12, indSize, activeColorSet.sliderEdge);
		graphicsBoxfA(&screenBitmap, 308,    indPos,         11, indSize, activeColorSet.sliderEdge);
		graphicsBoxfA(&screenBitmap, 308,    indPos+1,         12-2, indSize-2, activeColorSet.slider);
		graphicsBoxfA(&screenBitmap, 307,      indPos,            1, indSize, activeColorSet.sliderEdge);

/*
		graphicsBoxfA(&screenBitmap, 307+12-1, indPos,            1, indSize, activeColorSet.sliderEdge);
		graphicsBoxfA(&screenBitmap, 307,      indPos,           11,       1, activeColorSet.sliderEdge);
		graphicsBoxfA(&screenBitmap, 307,      indPos+indSize-1, 11,       1, activeColorSet.sliderEdge);
*/
		lastIndicatorDrawnSize = indSize;
		lastIndicatorDrawnPos = indPos;
	}


	while (drawChars > 0) {
		graphicsStringA(&screenVideo, 0, 0, activeFont, activeFont_, FONTSIZE_X, 0, (drawChars & 1) ? "x" : "X");
		drawChars--;
	}
}
void updateLine(int nr, int pos) {
	char buffer[10];
	int lineLength = lineLen(linePtr[nr]);
	char* lineStart = linePtr[nr];
	int lineIndex = lineLength - remaining[pos];
	while ((drawChars > 0) && (remaining[pos] > 0)) {
		buffer[0] = *(lineStart + lineIndex);
		buffer[1] = 0;
		graphicsStringA(&screenVideo2, 1 + lineIndex*FONTSIZE_X, pos*FONTSIZE_Y, activeFont, activeFont_, FONTSIZE_X, 0, buffer);
		drawChars--;
		remaining[pos]--;
		lineIndex++;
	}
}

void loadTextFile(char * fileName) {

	inifatinfo();
	inidir();
	inifile();

	ataSelectHDDA();
	ataPowerUpHDDA();

	ataReadMBR();
	fatInit(getPartition(0));

	int fileHandle;
//	fileHandle = fopen("/testfile.txt");

// fileHandle = fopen("/TESTFILE.TXT","r");
	fileHandle = fopen(fileName,"r");


//	fileHandle = fopen("/TESTFILE.TXT", "r");
//	fileHandle = fopen("/TESTFILE.TXT");
//	fileHandle = fopen("/TESTFI~1.TXT");
//	fileHandle = fopen("/testfi~1.txt");

	if (fileHandle < 0) {
		textFile[0] = 'E';
		textFile[1] = '0' - fileHandle;
	    ataPowerDownHDDA();

		return;
	}

	int fileSize = fsize(fileHandle);
	if (fileSize >= sizeof(textFile)) {
		fileSize = sizeof(textFile)-5;
	}

	if (fileSize > 5) {
		fread(fileHandle, textFile, fileSize);

		textFile[fileSize] = '\n';
		textFile[fileSize+1] = '\n';
		textFile[fileSize+2] = '.';
		textFile[fileSize+3] = '\n';
		textFile[fileSize+4] = 0;
	}

	fclose(fileHandle);

// Might as well power down HD and save battery :)
    ataPowerDownHDDA();
}


#ifdef	SHOULD_KEEP_ALL_LINEFEEDS
#define LINEFEED_LIMIT		0
#else
#define LINEFEED_LIMIT		1
#endif
void initTextReader(char * filename) {

	loadTextFile(filename);

	int indxIn = 0;
	int indxOut = 0;
	int remLineLength = AREA_COLUMNS;
	int currLine = 0;

	linePtr[currLine] = processedText;

	while (textFile[indxIn] != 0) {
		int nextWord = getWordLength(textFile + indxIn);
		if (nextWord < remLineLength) {		// fits on line
			copyN(textFile + indxIn, processedText + indxOut, nextWord);
			remLineLength -= nextWord;
			indxIn += nextWord;
			indxOut += nextWord;
		} else {
			processedText[indxOut++] = '\n';	// start new line
			linePtr[++currLine] = processedText + indxOut;
			remLineLength = AREA_COLUMNS;
			if (nextWord < AREA_COLUMNS) {	// place word on next line
				copyN(textFile + indxIn, processedText + indxOut, nextWord);
				remLineLength -= nextWord;
				indxIn += nextWord;
				indxOut += nextWord;
			} else {		// word too long for line, must break it
				copyN(textFile + indxIn, processedText + indxOut, AREA_COLUMNS-1);
				processedText[indxOut + AREA_COLUMNS-1] = '-';
				indxIn += AREA_COLUMNS-1;
				indxOut += AREA_COLUMNS;
				processedText[indxOut++] = '\n';	// fill and start new line
				linePtr[++currLine] = processedText + indxOut;
				remLineLength = AREA_COLUMNS;
			}
		}
		int skippedNewlines = 0;
		int whiteSpace = skipWhitespaceLength(textFile + indxIn, &skippedNewlines);
		indxIn += whiteSpace;
		if ((remLineLength > 0) && (remLineLength < AREA_COLUMNS)) {
			processedText[indxOut++] = ' ';
			remLineLength--;
		}
		if ((remLineLength == 0) || (skippedNewlines > LINEFEED_LIMIT)) {		// no space, just new line
			processedText[indxOut++] = '\n';	// start new line
			linePtr[++currLine] = processedText + indxOut;
			remLineLength = AREA_COLUMNS;
		}
		if (skippedNewlines > 1) {				// double new line if new paragraph
			processedText[indxOut++] = '\n';	// start new line
			linePtr[++currLine] = processedText + indxOut;
			remLineLength = AREA_COLUMNS;
		}
	}
	processedText[indxOut++] = '\n';	// finish the last line
	linePtr[++currLine] = processedText + indxOut;
	nrOfLines = currLine;

/*
	linePtr[0] = "ROW0\n";
	linePtr[1] = "ROW1\n";
	linePtr[2] = "ROW2\n";
	linePtr[3] = "ROW3\n";
	linePtr[4] = "ROW4\n";
	linePtr[5] = "ROW5\n";
	linePtr[6] = "ROW6\n";
	linePtr[7] = "ROW7\n";
	linePtr[8] = "ROW8\n";
	linePtr[9] = "ROW9\n";
	nrOfLines = 10;
*/
}

void initTextArea() {
	int i;

	for (i=0; i<AREA_LINES; i++) {
		reqLine[i] = (i < nrOfLines) ? i : nrOfLines-1;
		actualLine[i] = -1;
	}

	lastIndicatorDrawnSize = -1;
	lastIndicatorDrawnPos = -1;


    //draw a red border 2pix wide around edge of the 640x480 screenVideo2 layer (VIDEO2)
    graphicsBoxfA(&screenVideo2, 0, 0, 320, 480, activeColorSet.background);
/*
    graphicsBoxfA(&screenVideo2, 0, 0, 2, 480, 0xFFFF5050);
    graphicsBoxfA(&screenVideo2, 318, 0, 2, 480, 0xFFFF5050);
    graphicsBoxfA(&screenVideo2, 0, 0, 320, 2, 0xFFFF5050);
    graphicsBoxfA(&screenVideo2, 0, 478, 320, 2, 0xFFFF5050);
*/

    //fill center area with dark gray background
    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000); //gray is 0x12 on built-in pallette
//    graphicsBoxfA(&screenBitmap, 0, 0, 320, 13, 0xfafa); //green is 0xfa on built-in pallette
    graphicsBoxfA(&screenBitmap, 0, (VISIBLE_LINES-1)*FONTSIZE_Y, 320, 240-(VISIBLE_LINES-1)*FONTSIZE_Y, activeColorSet.bottom);  //(bottom edge)
    graphicsBoxfA(&screenBitmap, 0, (VISIBLE_LINES-1)*FONTSIZE_Y, 320, 1, activeColorSet.sliderEdge);  //(bottom edge)
//    graphicsBoxfA(&screenBitmap, 160, 220, 160, 20, 0x0000);  //(bottom edge)

    //enable bitmap1 component
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
									| OSD_BITMAP_0TRANS);
//                                     | OSD_BITMAP_MERGEBACK
//                                     | OSD_BITMAP_A6);
    //enable video2 component, we'll show that first
    osdSetComponentConfigA(OSD_VIDEO2, OSD_COMPONENT_ENABLE);

//	int scrSize = (VISIBLE_LINES-1)*FONTSIZE_Y;

	graphicsBoxfA(&screenVideo2, 315, 0, 4, 480, activeColorSet.foreground);
	graphicsBoxfA(&screenVideo2, 316, 0, 2, 480, activeColorSet.background);
}


int clampLine(int lineNr) {
	if (lineNr < 0) return 0;
	if (lineNr >= nrOfLines) return nrOfLines-1;
	return lineNr;
}

int XmodY(int x, int y) {
	while (x < 0) x += y;
	while (x >= y) x -= y;
	return x;
}

void runMainLoop() {
	int key;
	int layer = 0; 
	showTopLine = 0;
	int screenScrolled = 0;

	int tmp;

	int moveSwitch = 0;

	int targetTopLine = showTopLine;

	while (1) {
/*
		reqLine[screenScrolled+1] = clampLine(realTopLine+1);
		reqLine[screenScrolled+2] = clampLine(realTopLine+2);
		reqLine[screenScrolled+3] = clampLine(realTopLine+3);
		reqLine[screenScrolled+VISIBLE_LINES-1] = clampLine(realTopLine+VISIBLE_LINES-1);

		reqLine[XmodY(screenScrolled   +VISIBLE_LINES, AREA_LINES)] = clampLine(realTopLine);
		reqLine[XmodY(screenScrolled+1 +VISIBLE_LINES, AREA_LINES)] = clampLine(realTopLine+1);
		reqLine[XmodY(screenScrolled+2 +VISIBLE_LINES, AREA_LINES)] = clampLine(realTopLine+2);
		reqLine[XmodY(screenScrolled+3 +VISIBLE_LINES, AREA_LINES)] = clampLine(realTopLine+3);
		reqLine[XmodY(screenScrolled+VISIBLE_LINES-1 +VISIBLE_LINES, AREA_LINES)] = clampLine(realTopLine+VISIBLE_LINES-1);
*/
/*
		reqLine[(screenScrolled + VISIBLE_LINES) % AREA_LINES] = realTopLine;

		reqLine[((screenScrolled-1) % AREA_LINES)] = clampLine(realTopLine-1);
		reqLine[(screenScrolled + VISIBLE_LINES -1) % AREA_LINES] = clampLine(realTopLine-1);
*/
		key = buttonsGetStatusA();
		if (key & BUTTONS_AV300_OFF) {
			timeToQuit = 1;
			break;
		}
		if (key & BUTTONS_AV300_MENU1) {
			timeToQuit = 0;
			break;
		}

		int doSomething = 0;

		if (key & (BUTTONS_AV300_MENU2) ) {
			nrOfCharsToDrawPerUpdate = 1;
		} else {
			nrOfCharsToDrawPerUpdate = 45;
		}
		if ( ((key & BUTTONS_AV300_UP) && (moveSwitch == 0)) || ((key & BUTTONS_AV300_LEFT) && (moveSwitch == 1)) ) {
//			moveSwitch = 1 - moveSwitch;

			showTopLine -= 1;
			if (showTopLine < 0) {
				showTopLine = 0;
			}
			doSomething = 1;
		}
		if ( ((key & BUTTONS_AV300_DOWN) && (moveSwitch == 0)) || ((key & BUTTONS_AV300_RIGHT) && (moveSwitch == 1)) ) {
//			moveSwitch = 1 - moveSwitch;

			showTopLine += 1;
			if (showTopLine >= (nrOfLines - VISIBLE_LINES)) {
				showTopLine = (nrOfLines - VISIBLE_LINES);
			}
			if (showTopLine < 0) {
				showTopLine = 0;
			}
			doSomething = 1;
		}
		if (key & (BUTTONS_AV300_RIGHT | BUTTONS_AV300_MENU3 | BUTTONS_AV300_ON)) {
			if ((targetTopLine - showTopLine) < VISIBLE_LINES/2) {
				targetTopLine += (VISIBLE_LINES-2);
			}
			if (targetTopLine >= (nrOfLines - VISIBLE_LINES)) {
				targetTopLine = (nrOfLines - VISIBLE_LINES);
			}
			if (targetTopLine < 0) {
				targetTopLine = 0;
			}
		}
		if (key & BUTTONS_AV300_LEFT) {
			if ((showTopLine - targetTopLine) < VISIBLE_LINES/2) {
				targetTopLine -= (VISIBLE_LINES-2);
			}
			if (targetTopLine < 0) {
				targetTopLine = 0;
			}
		}
		if (doSomething == 0) {
			if (targetTopLine < showTopLine) {
				showTopLine--;
			}
			if (targetTopLine > showTopLine) {
				showTopLine++;
			}
			doSomething = 1;
		} else {
			targetTopLine = showTopLine;
		}
		if (doSomething == 1) {
			screenScrolled = XmodY(showTopLine, VISIBLE_LINES);

//		while (buttonsGetStatus() & (BUTTONS_AV300_UP|BUTTONS_AV300_DOWN));

/*
		while (showTopLine < realTopLine) {
			screenScrolled = XmodY(screenScrolled-1, VISIBLE_LINES);
			realTopLine -= 1;
		}
		while (showTopLine > realTopLine) {
			screenScrolled = XmodY(screenScrolled+1, VISIBLE_LINES);
			realTopLine += 1;
		}
*/		
			screen_y_req = screenScrolled * FONTSIZE_Y;

			int scrollDiff = screen_y_req - yv;
			if (scrollDiff < 0) {
				scrollDiff = -scrollDiff;
			}

			if (scrollDiff < 120) {
				reqLine[XmodY(screenScrolled, AREA_LINES)] = clampLine(showTopLine);
				reqLine[XmodY(screenScrolled + VISIBLE_LINES-1, AREA_LINES)] = clampLine(showTopLine+VISIBLE_LINES-1);
			} else {
				reqLine[XmodY(screenScrolled + VISIBLE_LINES, AREA_LINES)] = clampLine(showTopLine);
				reqLine[XmodY(screenScrolled + VISIBLE_LINES+VISIBLE_LINES-1, AREA_LINES)] = clampLine(showTopLine+VISIBLE_LINES-1);
			}

			updateScreen(screenScrolled);

			if (screen_y_req > yv) {
				while (screen_y_req != yv) {
					if (scrollDiff < 120) {
						doScroll(0, 1);
					} else {
						doScroll(0, -1);		// scroll reverse direction for shortest way including wrap
					}
					if (targetTopLine == showTopLine) {
						delay(0x800);
					}
				}
			}
			if (screen_y_req < yv) {
				while (screen_y_req != yv) {
					if (scrollDiff < 120) {
						doScroll(0, -1);
					} else {
						doScroll(0, 1);
					}
					if (targetTopLine == showTopLine) {
						delay(0x800);
					}
				}
			}

			if (scrollDiff < 120) {
				reqLine[XmodY(screenScrolled + VISIBLE_LINES, AREA_LINES)] = clampLine(showTopLine);
				reqLine[XmodY(screenScrolled + VISIBLE_LINES+VISIBLE_LINES-1, AREA_LINES)] = clampLine(showTopLine+VISIBLE_LINES-1);
			} else {
				reqLine[XmodY(screenScrolled, AREA_LINES)] = clampLine(showTopLine);
				reqLine[XmodY(screenScrolled + VISIBLE_LINES-1, AREA_LINES)] = clampLine(showTopLine+VISIBLE_LINES-1);
			}
		}

		updateScreen(screenScrolled);
/*
		for (tmp=0; tmp<VISIBLE_LINES; tmp++) {
			reqLine[screenScrolled + tmp] = clampLine(showTopLine+tmp);
		}
*/


/*
		if (key & BUTTONS_AV300_UP) doScroll(0,-1);
		if (key & BUTTONS_AV300_RIGHT) doScroll(1,0);
		if (key & BUTTONS_AV300_DOWN) doScroll(0,1);
		if (key & BUTTONS_AV300_LEFT) doScroll(-1,0);
*/
//		delay(0x0800);
	}
}

 int colorRGB2Packed(int r, int g, int b) {
int y = 299*r + 587*g + 114*b;
int cb = -169*r + -331*g + 500*b + 128000;
int cr = 500*r - 419*g - 81*b + 128000;
y = (y+500)/1000;
cb = (cb+500)/1000;
cr = (cr+500)/1000;
if (y < 0) y = 0;
if (y > 255) y = 255;
if (cb < 0) cb = 0;
if (cb > 255) cb = 255;
if (cr < 0) cr = 0;
if (cr > 255) cr = 255;
return 0x80000000 | cr << 16 | y << 8 | cb;
}
