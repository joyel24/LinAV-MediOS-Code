// clear the lcd
#define lcd_clear_display(window, color) GrFillRect(window, color, 0, 0, 320, 240)

// print text
#define lcd_putsxy(window, color, x, y, text) GrText(window, color, x, y, text, -1, GR_TFASCII)

// draw a rectangle
#define lcd_drawrect(window, color, x, y, w, h) GrRect(window, color, x, y, w, h)

// fill a rectangle
#define lcd_fillrect(window, color, x, y, w, h) GrFillRect(window, color, x, y, w, h)

// draw a line
#define lcd_drawline(window, color, x, y, x2, y2) GrLine(window, color, x, y, x2, y2)

// draw a pixel
#define lcd_drawpixel(window, color, x, y) GrPoint(window, color, x, y)

// draw a circle
#define lcd_drawcircle(window, color, x, y, w, h) GrEllipse(window, color, x, y, w, h)

// fill a circle
#define lcd_fillcircle(window, color, x, y, w, h) GrFillEllipse(window, color, x, y, w, h)