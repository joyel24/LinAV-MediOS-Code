#include <graphics.h>
#include <buttons.h>
#include <osdDSC25.h>
#include <math.h>

void plot(float x, float y, int c);
int escapes( float i, float j );

    static int pal32[2] = {
        0x8080c0e0, 0xffffffff
    };
    static int pal16[2] = {
        0x0000, 0xffff
    };

    struct graphicsBuffer screenBitmap;
    
    unsigned int pallette[256];
    

int main() {
    int c, v, b;
    int cpalo=0, cp=0;
    osdInitA();

    for (c=0;c<256;c++) {
        pallette[c] = v;
        v += 0x036e8a;
    }
    
    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);    
    
    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_RAMCLUT);

    cp = 0;
    for (c=1;c<256;c++) {
        osdSetPalletteA(pallette[cp], pallette[cp]>>8, pallette[cp]>>16, c);
        cp++;
        cp &= 0xff;
    }

  float i, j;
  for( i = -2; i < 1; i = i + ((float)2.8/320)) {
    for( j = -1.5; j < 1.5; j = j + ((float)2.8/240)) {
      plot( i+0.5, j, escapes( i, j ) );
      b = buttonsGetStatusA();
      if (b & BUTTONS_AV300_OFF) return;
    }
  }
  
  int delay;
  while(1) {
    cpalo++;
    cpalo &= 0xff;
    cp = cpalo;
    for (c=1;c<256;c++) {
        osdSetPalletteA(pallette[cp], pallette[cp]>>8, pallette[cp]>>16, c);
        cp++;
        cp &= 0xff;
    }

    for(delay=0;delay<0xc000;delay++) {
        b = buttonsGetStatusA();
        if (b & BUTTONS_AV300_OFF) return;
    }
  }

}

int escapes( float i, float j ) {
  float a = i;
  float b = j;
  float tempx, tempy;
  int n;
  for( n = 0; n < 256; n++ ) {
    tempx = a*a;
    tempy = b*b;
    if( (tempx + tempy) > 2 ) {
      return n;
    }
    tempx = tempx - tempy + i;
    tempy = 2*a*b + j;
    a = tempx;
    b = tempy;
  }
  return 0;
}

void plot(float x, float y, int c) {
    unsigned int dx, dy, dc;
    dx = 160 + x * 106;
    dy = 120 + y * 80;

    graphicsSetPixelA(&screenBitmap, dx, dy, c);

}
