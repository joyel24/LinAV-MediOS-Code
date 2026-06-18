/* alias.h
 * aliases for functions/variables */

/*****************
 * DRAWING ALIASES
 ****************/

// draw a pixel
#define lcd_drawpixel(color, x, y) drawPixel(color, x, y)

// read a pixel
#define lcd_readpixel(x, y) readPixel(x, y)

// draw a rectangle
#define lcd_drawrect(color, x, y, w, h) drawRect(color, x, y, w, h)

// fill a rectangle
#define lcd_fillrect(color, x, y, w, h) fillRect(color, x, y, w, h)

// draw a line
#define lcd_drawline(color, x, y, x2, y2) drawLine(color, x, y, x2, y2)

// clear the screen
#define lcd_clear_display(color) fillRect(color, 0, 0, 320, 240)

// put some text on the screen
#define lcd_putsxy(color, bgcolor, x, y, text) putS(color, bgcolor, x, y, text)

// draw a sprite
#define lcd_drawsprite(palette, sprite, x, y) drawSprite(palette, sprite, x, y)

// draw a bitmap
#define lcd_bitmap(bitmap, x, y) drawBITMAP(bitmap, x, y)

// set the font
#define lcd_setfont(newfont) setFont(newfont)

/******************
 * VARIABLE ALIASES
 *****************/

// down key
#define BUTTON_DOWN BTN_DOWN

// up key
#define BUTTON_UP BTN_UP

// right key
#define BUTTON_RIGHT BTN_RIGHT

// left key
#define BUTTON_LEFT BTN_LEFT

// joy key
#define BUTTON_JOY BTN_JOY

// on key
#define BUTTON_ON BTN_ON

// off key
#define BUTTON_OFF BTN_OFF

// F1 key
#define BUTTON_F1 BTN_F1

// F2 key
#define BUTTON_F2 BTN_F2

// F3 key
#define BUTTON_F3 BTN_F3
