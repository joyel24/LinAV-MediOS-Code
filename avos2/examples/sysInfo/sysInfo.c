#include <buttons.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <fonts.h>
#include <cpld.h>
#include <stdio.h>
#include <mas.h>

char* cpldVer = "CPLD Version: xxxxxxxx";
char* modID = "Module ID:    xxxxxxxx";
char* masVer = "MAS  Version: xxxxxxxx";

struct graphicsBuffer screenBitmap;
static int pal16[2] = {0x0000, 0xffff};
struct graphicsBuffer sprite5_8 = {0, 1, 5, 8, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, 0};

int main() {
    unsigned int a;
    
    osdInitA();

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
                                     | OSD_BITMAP_8BIT);

    graphicsStringA(&screenBitmap, 0, 0, &sprite5_8, std5x8_, 6, 0,
                "SysInfo v0.01 (c)DoGgEr");

    cpldSetModeA(CPLD_MODE_DEFAULT);
    masResetA();            
    
    sprintf(cpldVer, "CPLD Version: %08X", cpldGetVersionA());
    graphicsStringA(&screenBitmap, 0, 9, &sprite5_8, std5x8_, 6, 0,
                cpldVer);

    sprintf(modID, "Module ID:    %08X", cpldGetModuleIDA());
    graphicsStringA(&screenBitmap, 0, 18, &sprite5_8, std5x8_, 6, 0,
                modID);
                
    sprintf(masVer, "MAS  Version: %08X", masGetVersionA());
    graphicsStringA(&screenBitmap, 0, 27, &sprite5_8, std5x8_, 6, 0,
                masVer);
                
    while(1) {
        a = buttonsGetStatusA();
        if (a & BUTTONS_AV300_OFF) return 0;
    }
}
