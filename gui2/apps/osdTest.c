#include "graphics.h"
#include "events.h"
#include "osd.h"

    static int pal32[2] = {0x8080c0e0, 0xffffffff};
    static int pal16[2] = {0x0101, 0xffff};


 /************
 * Fonts used
 ***********/
needFont(std4x6);
needFont(std5x8);
needFont(std7x13);
needFont(shadow);
needFont(cursive);
     
 /*   static struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    static struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    static struct graphicsBuffer sprite7_13 = {0, 1, 7, 13, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    static struct graphicsBuffer spriteShadow = {0, 2, 12, 18, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    static struct graphicsBuffer spriteCursive = {0, 2, 9, 15, 1, 0, 0, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
*/
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
    int fadec=0;
    int movec=0;
    int cellc=0;
    int cursorx=0, cursory=0;
    int butslow = 160;
    int butmax = 160;
    int c;
    int evt;
    
    int mvzoom = 0;
    int mvlatched = 0;
    
    int x=32, y=32, cdir=0;
    
    printf("in osdTest\n");
    
    ini_graphics();
    
    showPlane(VID1);
    
        
    setSize(VID2,64,64,32);
    showPlane(VID2);
    
    setSize(BMAP1,160,100,8);
    setState(BMAP1,AV3XX_OSD_BITMAP_8BIT
             | AV3XX_OSD_BITMAP_MERGEBACK
             | AV3XX_OSD_BITMAP_A4
             | AV3XX_OSD_BITMAP_ZX1);
    showPlane(BMAP1);    

    setSize(BMAP2,32,32,8);
    setState(BMAP2,AV3XX_OSD_BITMAP_8BIT
             | AV3XX_OSD_BITMAP_MERGEBACK
             | AV3XX_OSD_BITMAP_A4
             | AV3XX_OSD_BITMAP_ZX1);
    showPlane(BMAP2);
    
    setState(CUR2,AV3XX_OSD_CURSOR2_A0);
    showPlane(CUR2);
   

 #ifdef AV_SCREEN                                    
    for (c=0;c<16;c++) {
        osdSet16CPallette(0, c, c);
    }
    
    for (c=0;c<256;c++) {
        osdSetCursor2Bitmap(c, cursor[c]);    
    }
#endif    
    setPlane(VID1);
    printf("fill 1\n");
    fillRect(0x00000000, 0, 0, 320, 240);
    printf("fill 2\n");
    fillRect(0x80800000, 0, 0, 160, 120);
    printf("fill 3\n");
    fillRect(0xc0c0c000, 20, 120, 200, 100);
   
    
    setPlane(VID2);
    fillRect(0xf0f0d0e0, 0, 0, 64, 64);

    setPlane(BMAP1);
    fillRect(0x00, 0, 0, 160, 100);
    fillRect(0x01, 20, 20, 120, 80);
    fillRect(0x04, 0, 0, 40, 32);
    setFont(std4x6);
    putS(0x01, 0xff, 2, 20,"abcdefghijklmnop");
    
    setPlane(BMAP2);
    setFont(shadow);
    putS(0x01, 0xff, 0, 0,"D");
    
    setPlane(VID1);
    printf("fill 4\n");
    fillRect(0xc0c0c000, 80, 60, 100, 100);
    printf("setfont 1\n");
    setFont(shadow);
    printf("putS 1\n");
    putS(0x8080c0e0, 0xffffffff, 2, 120,"ABC DEF GHI JKL MNOPQ");
    printf("loop\n");
    setFont(std5x8);
    putS(0x8080c0e0, 0xffffffff, 2, 48," .,<>/?;':[]{}|=-+_)(*&^%$#@!");
    setFont(std7x13);
    putS(0x8080c0e0, 0x50503020, 100, 30,"Font drawing!");
    setFont(cursive);
    putS(0x8080c0e0, 0x202090a0, 4, 20,"BY OxyGen77");

    while(1) {
        movec++;
        if (movec>0x800) {
            movec=0;
            setPos(BMAP1,0x14 + (bx*2), 0x13 + by);
            setPos(BMAP2,0x14 + (bx*2)+ (bx2*2), 0x13 + by+ by2);
            setPos(VID2,0x14 + (vx*2), 0x13 + vy);
            
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
            setState(BMAP1,AV3XX_OSD_BITMAP_8BIT
                     | AV3XX_OSD_BITMAP_MERGEBACK
                     | (bdf << AV3XX_OSD_BITMAP_A_SHIFT)
                     | AV3XX_OSD_BITMAP_ZX1);
            showPlane(BMAP1);       
        }

        if (butslow==0)
        {
            butslow=butmax;
            evt=nxtEvent();
            switch(evt)
            {
                case BTN_UP:
                    cursory--;
                    butmax-=2;
                    break;
                case BTN_DOWN:
                    cursory++;
                    butmax-=2;
                    break;
                case BTN_LEFT:
                    cursorx--;
                    butmax-=2;
                    break;
                case BTN_RIGHT:
                    cursorx++;
                    butmax-=2;
                    break;
                default:
                     butmax = 160;   
                case BTN_F1:                    
                    break;
                case BTN_F2:                    
                    break;
                case BTN_F3:
                // some code zooming on video plane
                    break;
                case BTN_OFF:
                /*nothing to do => returning */
                    return 0;            
            
            /*if (!(c & BUTTONS_AV300_MENU3)) mvlatched=0;*/
            }                       
            if (butmax<30) butmax=30;
        
        

        if (cursorx<0) cursorx=0;
        if (cursory<0) cursory=0;
        if (cursorx>(318)) cursorx=318;
        if (cursory>(238)) cursory=238;

        setPos(CUR2,(cursorx<<1)+0x18, cursory+0x13);
        }
        
        butslow--;
        
        cellc++;
        if (cellc==0x80) {
            cellc=0;
            setPlane(VID2);
            c = readPixel(x,y);
            if ((c&1)==0) cdir++; else cdir--;
            cdir = cdir & 3;
            c=c + 0x010305;
            drawPixel(c,x,y);
        
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
