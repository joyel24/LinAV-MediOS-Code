#include <graphics.h>
#include <osdDSC25.h>

int main() {
    int c=1, b;
    int x, y;
    struct graphicsBuffer screenVideo1;
    struct graphicsBuffer screenVideo2;
    int cbuffer=0;
    
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

    screenVideo1.offset = 0x03c00000;
    screenVideo1.bytesPerLine = 320*4;
    screenVideo1.width = 320;
    screenVideo1.height = 240;
    screenVideo1.bitsPerPixelShift = 5;
    screenVideo1.bitsPerPixel = 32;

    screenVideo2.offset = 0x03800000;
    screenVideo2.bytesPerLine = 320*4;
    screenVideo2.width = 320;
    screenVideo2.height = 240;
    screenVideo2.bitsPerPixelShift = 5;
    screenVideo2.bitsPerPixel = 32;
    
    while(1) {
        if (cbuffer) {
            osdSetComponentOffset(OSD_VIDEO1, screenVideo2.offset);    
        } else {
            osdSetComponentOffset(OSD_VIDEO1, screenVideo1.offset);
        }
        
    c=c*2;
    for (y=0;y<240;y++) {
        for (x=0;x<320;x++) {
            b = (x-160)*(y-120)*c >> 4;
            if (cbuffer) {
                graphicsSetPixel(&screenVideo1, x, y, b);
            } else {
                graphicsSetPixel(&screenVideo2, x, y, b);                
            }
        }
    }

    cbuffer = !cbuffer;

    }
}
