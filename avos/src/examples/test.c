#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>

    static int pal32[2] = {
        0x8080c0e0, 0xffffffff
    };
    static int pal16[2] = {
        0x0101, 0xffff
    };

    struct graphicsBuffer screenVideo;
    struct graphicsBuffer screenVideo2;
    struct graphicsBuffer screenBitmap;
    struct graphicsBuffer screenBitmap2;
    
    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    struct graphicsBuffer sprite7_13 = {0, 1, 7, 13, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    struct graphicsBuffer spriteShadow = {0, 2, 12, 18, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    struct graphicsBuffer spriteCursive = {0, 2, 9, 15, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};

    int cursor[256] = {
        0xbb00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbbb, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xbb00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaabb, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xbb00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xaabb, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xaaaa, 0xbb00, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xaaaa, 0xaabb, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xaaaa, 0xaaaa, 0xbb00, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xaaaa, 0xaaaa, 0xaabb, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xaaaa, 0xaaaa, 0xbbbb, 0xbbbb, 0xbb00, 0x0000, 0x0000,
        0xbbaa, 0xaabb, 0xbbaa, 0xbb00, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbaa, 0xbb00, 0xbbaa, 0xbb00, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbbbb, 0x0000, 0x00bb, 0xaabb, 0x0000, 0x0000, 0x0000, 0x0000,
        0xbb00, 0x0000, 0x00bb, 0xaabb, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0xbbaa, 0xbb00, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0xbbbb, 0xbb00, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
    
//
//
//
int main() {
    int bx=0, by=0, bdx=1, bdy=1, bdf=0, dbdf=1;    
    int bx2=0, by2=0, bdx2=2, bdy2=2;
    int vx=160, vy=0, vdx=-1, vdy=1; 
    int delay;
    int fadec=0;
    int movec=0;
    int cellc=0;
    int cursorx=0, cursory=0;
    int butslow = 160;
    int butmax = 160;
    int c;
    
    int mvzoom = 0;
    int mvlatched = 0;
    
    int x=32, y=32, cdir=0;
    
    osdInit();
    
    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);
    
    osdSetComponentSize(OSD_VIDEO1, 640, 240);
    osdSetComponentPosition(OSD_VIDEO1, 0x14, 0x13);
    osdSetComponentOffset(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidth(OSD_VIDEO1, 0x28);
    osdSetComponentConfig(OSD_VIDEO1, OSD_COMPONENT_ENABLE);

    osdSetComponentSize(OSD_VIDEO2, 128, 64);
    osdSetComponentPosition(OSD_VIDEO2, 0x14, 0x13);
    osdSetComponentOffset(OSD_VIDEO2, 0x03e00000);
    osdSetComponentSourceWidth(OSD_VIDEO2, 0x28);
    osdSetComponentConfig(OSD_VIDEO2, OSD_COMPONENT_ENABLE);

    osdSetComponentSize(OSD_BITMAP1, 300, 100);
    osdSetComponentPosition(OSD_BITMAP1, 0x14, 0x13);
    osdSetComponentOffset(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x14);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A4);

    osdSetComponentSize(OSD_BITMAP2, 64, 32);
    osdSetComponentPosition(OSD_BITMAP2, 0x14, 0x13);
    osdSetComponentOffset(OSD_BITMAP2, 0x03700000);
    osdSetComponentSourceWidth(OSD_BITMAP2, 0x3);
    osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A4);

    osdSetComponentConfig(OSD_CURSOR2, OSD_COMPONENT_ENABLE
                                     | OSD_CURSOR2_A0
                                     );

                                     
    for (c=0;c<16;c++) {
        osdSet16CPallette(0, c, c);
    }
    
    for (c=0;c<256;c++) {
        osdSetCursor2Bitmap(c, cursor[c]);    
    }
    
    screenVideo.offset = 0x03c00000;
    screenVideo.bytesPerLine = 320*4;
    screenVideo.width = 320;
    screenVideo.height = 240;
    screenVideo.bitsPerPixelShift = 5;
    screenVideo.bitsPerPixel = 32;

    screenVideo2.offset = 0x03e00000;
    screenVideo2.bytesPerLine = 320*4;
    screenVideo2.width = 64;
    screenVideo2.height = 64;
    screenVideo2.bitsPerPixelShift = 5;
    screenVideo2.bitsPerPixel = 32;
    
    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 150;
    screenBitmap.height = 100;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    screenBitmap2.offset = 0x03700000;
    screenBitmap2.bytesPerLine = 48*2;
    screenBitmap2.width = 40;
    screenBitmap2.height = 32;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;
    
    graphicsBoxf(&screenVideo, 0, 0, 320, 240, 0x000000);
    graphicsBoxf(&screenVideo, 0, 0, 160, 120, 0x80800000);
    graphicsBoxf(&screenVideo, 20, 120, 200, 100, 0xc0c0c000);

    graphicsBoxf(&screenVideo2, 0, 0, 64, 64, 0xf0f0d0e0);

    graphicsBoxf(&screenBitmap, 0, 0, 160, 120, 0x0000);
    graphicsBoxf(&screenBitmap, 20, 20, 120, 80, 0x0101);
    graphicsBoxf(&screenBitmap2, 0, 0, 40, 32, 0x0404);
    
    graphicsString(&screenBitmap, 2, 20, &sprite4_6, std4x6_, 5, 1, "abcdefghijklmnop");

    graphicsString(&screenBitmap2, 0, 0, &spriteShadow, shadow_, 12, 0, "DOG");

    graphicsBoxf(&screenVideo, 80, 60, 100, 100, 0xc0c0c000);   //0x4040e050);
    
    
    graphicsString(&screenVideo, 2, 120, &spriteShadow, shadow_, 13, 0, "ABC DEF GHI JKL MNOPQ");
    graphicsString(&screenVideo, 2, 48, &sprite5_8, std5x8_, 6, 1, " .,<>/?;':[]{}|=-+_)(*&^%$#@!");
    pal32[1] = 0x50503020;
    graphicsString(&screenVideo, 100, 30, &sprite7_13, std7x13_, 0, 14, "Font drawing!");
    pal32[1] = 0x202090a0;
    graphicsString(&screenVideo, 4, 200, &spriteCursive, cursive_, 10, 0, "BY DoggerMoore");

    while(1) {
        movec++;
        if (movec>0x800) {
            movec=0;
            osdSetComponentPosition(OSD_BITMAP1, 0x14 + (bx*2), 0x13 + by);
            osdSetComponentPosition(OSD_BITMAP2, 0x14 + (bx*2) + (bx2*2), 0x13 + by + by2);
            osdSetComponentPosition(OSD_VIDEO2, 0x14 + (vx*2), 0x13 + vy);
            bx+=bdx;
            by+=bdy;
            if (bx<0) {bx=0;bdx=-bdx;}
            if (by<0) {by=0;bdy=-bdy;}
            if (bx>(319-150)) {bx=319-150;bdx=-bdx;}
            if (by>(239-100)) {by=239-100;bdy=-bdy;}

            bx2+=bdx2;
            by2+=bdy2;
            if (bx2<0) {bx2=0;bdx2=-bdx2;}
            if (by2<0) {by2=0;bdy2=-bdy2;}
            if (bx2>(149-40)) {bx2=149-40;bdx2=-bdx2;}
            if (by2>(99-32)) {by2=99-32;bdy2=-bdy2;}

            vx+=vdx;
            vy+=vdy;
            if (vx<0) {vx=0;vdx=-vdx;}
            if (vy<0) {vy=0;vdy=-vdy;}
            if (vx>(319-64)) {vx=319-64;vdx=-vdx;}
            if (vy>(239-64)) {vy=239-64;vdy=-vdy;}
        }
        
        fadec++;
        if (fadec>0x4000) {
            fadec=0;
            bdf+=dbdf;
            if (bdf==8) {bdf=7;dbdf=-dbdf;}
            if (bdf<0) {bdf=0;dbdf=-dbdf;}
            osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | (bdf << OSD_BITMAP_A_SHIFT));
        }

        if (butslow==0) {
            butslow=butmax;
            c = buttonsGetStatus();
            if (c & BUTTONS_AV300_UP) {
                cursory--;
                butmax-=2;
            } else if (c & BUTTONS_AV300_DOWN) {
                cursory++;
                butmax-=2;
            } else if (c & BUTTONS_AV300_LEFT) {
                cursorx--;
                butmax-=2;
            } else if (c & BUTTONS_AV300_RIGHT) {
                cursorx++;
                butmax-=2;
            } else {
                butmax = 160;
            }

            if (c & BUTTONS_AV300_MENU1) {
                osdSetComponentConfig(OSD_CURSOR2, OSD_COMPONENT_ENABLE
                                     | OSD_CURSOR2_A0
                                     | OSD_CURSOR2_ZY1 | OSD_CURSOR2_ZX1);
                osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A4
                                     | OSD_BITMAP_ZY2 | OSD_BITMAP_ZX2);
                osdSetComponentSize(OSD_BITMAP2, 128*2, 128);
            } else if (c & BUTTONS_AV300_MENU2) {
                osdSetComponentConfig(OSD_CURSOR2, OSD_COMPONENT_ENABLE
                                     | OSD_CURSOR2_A0);
                osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A4);
                osdSetComponentSize(OSD_BITMAP2, 32*2, 32);
            } else if (c & BUTTONS_AV300_MENU3) {
                if (!mvlatched) {
                mvlatched = 1;
                mvzoom++;
                if (mvzoom==3) mvzoom=0;
                if (mvzoom==0) {
                    osdSetComponentOffset(OSD_VIDEO1, 0x03c00000);
                    osdSetComponentConfig(OSD_VIDEO1, OSD_COMPONENT_ENABLE);                    
                } else if (mvzoom==1) {
                    osdSetComponentOffset(OSD_VIDEO1, 0x03c00000
                                                      + (80*4)
                                                      + (60*320*4));
                    osdSetComponentConfig(OSD_VIDEO1, OSD_COMPONENT_ENABLE
                                            | OSD_VIDEO_ZX1 | OSD_VIDEO_ZY1);                                        
                } else if (mvzoom==2) {
                    osdSetComponentOffset(OSD_VIDEO1, 0x03c00000
                                                      + (120*4)
                                                      + (90*320*4));
                    osdSetComponentConfig(OSD_VIDEO1, OSD_COMPONENT_ENABLE
                                            | OSD_VIDEO_ZX2 | OSD_VIDEO_ZY2);                    
                }
                }
            }
            
            if (!(c & BUTTONS_AV300_MENU3)) mvlatched=0;
                                     
            if (butmax<30) butmax=30;
        }
        butslow--;

        if (cursorx<0) cursorx=0;
        if (cursory<0) cursory=0;
        if (cursorx>(318)) cursorx=318;
        if (cursory>(238)) cursory=238;

        osdSetComponentPosition(OSD_CURSOR2, (cursorx<<1)+0x18, cursory+0x13);
        
        cellc++;
        if (cellc==0x80) {
            cellc=0;
            c = graphicsGetPixel(&screenVideo2, x, y);
            if ((c&1)==0) cdir++; else cdir--;
            cdir = cdir & 3;
            c=c + 0x010305;
            graphicsSetPixel(&screenVideo2, x, y, c);
        
            if (cdir==0) x++;
            else if (cdir==1) y++;
            else if (cdir==2) x--;
            else if (cdir==3) y--;
        
            if (x>63) x = x - 64;
            if (y>63) y = y - 64;
            if (x<0) x = x + 64;
            if (y<0) y = y + 64;
        }
        
    }
}
