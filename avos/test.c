#include <graphics.h>
#include <osdDSC25.h>

int main() {
    int cdir=0, c=0;
    int x=160, y=120;
    struct graphicsBuffer screenVideo;
    int px, py;
    
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

    screenVideo.offset = 0x03c00000;
    screenVideo.bytesPerLine = 320*4;
    screenVideo.width = 320;
    screenVideo.height = 240;
    screenVideo.bitsPerPixel = 32;

    for (py=0;py<240;py+=8) {
        for (px=0;px<320;px+=8) {
            c = (px-160)*(py-120);
            graphicsBoxf(&screenVideo, px, py, 8, 8, c);
        }
    }
    
    while(1) {
        c = graphicsGetPixel(&screenVideo, x, y);
        if ((c&1)==0) cdir++; else cdir--;
        cdir = cdir & 3;
        c=c + 0x010305;
        graphicsSetPixel(&screenVideo, x, y, c);
        
        if (cdir==0) x++;
        else if (cdir==1) y++;
        else if (cdir==2) x--;
        else if (cdir==3) y--;
        
        if (x>319) x = x - 320;
        if (y>239) y = y - 240;
        if (x<0) x = x + 320;
        if (y<0) y = y + 240;

    }
}
