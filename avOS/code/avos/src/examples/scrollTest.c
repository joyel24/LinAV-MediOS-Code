#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <ata.h>
#include <fonts.h>

void updateBuffer(int x, int y, int w, int h, int vx, int vy);
void moveViewport(int dx, int dy);
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

struct graphicsBuffer screenBitmap;

char stat[] = "xxxx xxxx xxxx xxxx";

int virtualx=0, virtualy=0;             //

int bufferx=0, buffery=0;               //  viewport inside buffer offset

int main() {
    unsigned int b, s;

    int dx=0, dy=0;
    
    ataPowerDownHDDA();
    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 656*2;          // Make it twice the screen + a bit
    screenBitmap.width = 656;
    screenBitmap.height = 496;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, 656, 496, 0x0000);

    updateBuffer(0, 0, 320, 240, 0, 0);

    osdSetMainShiftA(0x8d - 0x1e, 0x13);

    osdSetComponentSizeA(OSD_BITMAP1, 336*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x1e, 0);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x29);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_0TRANS
                                     | OSD_BITMAP_A1
                                     | OSD_BITMAP_8BIT);

    
    while(1) {    

        dx = 0;
        dy = 0;
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;
        if (b & BUTTONS_AV300_RIGHT) {
            dx=s;
            if (s<16) s++;
//            while(buttonsGetStatus() & BUTTONS_AV300_ANY) {}            
        } else if (b & BUTTONS_AV300_LEFT) {
            if (virtualx>=s) {
                dx=-s;
            } else {
                dx=-virtualx;
            }
            if (s<16) s++;
//            while(buttonsGetStatus() & BUTTONS_AV300_ANY) {}
        } else if (b & BUTTONS_AV300_DOWN) {
            dy=s;
            if (s<16) s++;
//            while(buttonsGetStatus() & BUTTONS_AV300_ANY) {}            
        } else if (b & BUTTONS_AV300_UP) {
            if (virtualy>=s) {
                dy=-s;
            } else {
                dy=-virtualy;
            }
            if (s<16) s++;
//            while(buttonsGetStatus() & BUTTONS_AV300_ANY) {}
        } else {
            s = 3;              // Reset speed...
        }
     
        if (dx!=0 || dy!=0) {
            moveViewport(dx, dy);
            //for (b=0;b<0x1000;b++) {}
        }
    }
}

void updateBuffer(int x, int y, int w, int h, int vx, int vy) {
    int cx, cy;
    unsigned int c;
    for (cx=0;cx<w;cx++) {
        for (cy=0;cy<h;cy++) {
//            c = (((320-(vx+cx))*(320-(vx+cx)))
//               + ((240-(vy+cy))*(240-(vy+cy)))) >> 8;

// Highly unoptomised...
            c = ((vy+cy)>>4)*((vx+cx)>>4);
            graphicsSetPixelA(&screenBitmap, x+cx, y+cy, c);        
        }
    }
    
}

void moveViewport(int dx, int dy) {
    int tx, ty;
    virtualx+=dx;
    virtualy+=dy;

    bufferx+=dx;
    buffery+=dy;

    stringPutHexA(stat, virtualx, 4);
    stringPutHexA(stat + 5, virtualy, 4);
    stringPutHexA(stat + 10, bufferx, 4);
    stringPutHexA(stat + 15, buffery, 4);
    graphicsStringA(&screenBitmap, 20, 20, &sprite8_13, std8x13_, 8, 0, stat);

    // ok, now work out what we need to draw...
    if (dx>0) {         // Draw the bit to the right...
        updateBuffer(bufferx-dx+320, buffery, dx, 240, virtualx-dx+320, virtualy);
    } else if (dx<0) {  // Draw the bit to the left...
        updateBuffer(bufferx, buffery, -dx, 240, virtualx, virtualy);
    }

    // ok, now work out what we need to draw...
    if (dy>0) {         // Draw the bit down...
        updateBuffer(bufferx, buffery-dy+240, 320, dy, virtualx, virtualy-dy+240);
    } else if (dy<0) {  // Draw the bit up...
        updateBuffer(bufferx, buffery, 320, -dy, virtualx, virtualy);
    }

    // ok, now handle edge cases...
    // TODO - These draw on currently displaying buffer if dx/dy > 16
    if (bufferx<0) {
        bufferx+=336;
        updateBuffer(bufferx, buffery, -dx, 240, virtualx, virtualy);
    } else if (bufferx>=336) {
        bufferx-=336;
        updateBuffer(bufferx+320-dx, buffery, dx, 240, virtualx+320-dx, virtualy);
    }
    
    if (buffery<0) {
        buffery+=256;
        updateBuffer(bufferx, buffery, 320, -dy, virtualx, virtualy);
    } else if (buffery>=256) {
        buffery-=256;
        updateBuffer(bufferx, buffery+240-dy, 320, dy, virtualx, virtualy+240-dy);
    }

    // Now set screen position within buffer...
    osdSetComponentPositionA(OSD_BITMAP1, 0x1e - ((bufferx&0xf)<<1), 0);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000 + ((bufferx&0xfffffff0)*2) + (buffery*2*656));

    // Now update the bit we just left for jumpback...
    if (dx>0) {         // Draw the bit to the LEFT just gone...
        updateBuffer(bufferx-dx, buffery, dx, 240, virtualx-dx+336, virtualy);
    } else if (dx<0) {  // Draw the bit to the RIGHT just gone...
        updateBuffer(bufferx+320, buffery, -dx, 240, virtualx-16, virtualy);
    }

    // Now update the bit we just left for jumpback...
    if (dy>0) {         // Draw the bit to the UP just gone...
        updateBuffer(bufferx, buffery-dy, 320, dy, virtualx, virtualy-dy+256);
    } else if (dy<0) {  // Draw the bit to the DOWN just gone...
        updateBuffer(bufferx, buffery+240, 320, -dy, virtualx, virtualy-16);
    }
    
    // TODO - Doesn't update diagonals...
    // Dammit more to do!

}
