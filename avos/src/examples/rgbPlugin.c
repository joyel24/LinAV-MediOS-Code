#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <ata.h>
#include <gio.h>
#include <file.h>
#include <debug.h>

int colorRGB2Packed(int r, int g, int b);

struct graphicsBuffer screenVideo;

static char imBuff[320*240*3];
static unsigned int * screenDirect = (unsigned int*) 0x03c00000;

int main(int argc, char * * argv) {
    int i, j, r, g, b, v, c, vmode=0;

	inifatinfo();
	inidir();
	inifile();
	ataSelectHDDA();
	ataPowerUpHDDA();
	ataReadMBR();
	fatInit(getPartition(0));

	int fileHandle;
	fileHandle = fopen(argv[1], "r");

	if (fileHandle < 0) return;

	int fileSize = fsize(fileHandle);
	if (fileSize != 320*240*3) return;

    fread(fileHandle, imBuff, fileSize);
	fclose(fileHandle);

    ataPowerDownHDDA();
   
    screenVideo.offset = 0x03c00000;
    screenVideo.bytesPerLine = 320*4;
    screenVideo.width = 320;
    screenVideo.height = 240;
    screenVideo.bitsPerPixelShift = 5;
    screenVideo.bitsPerPixel = 32;
    osdInitA();
    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    osdSetComponentSizeA(OSD_VIDEO1, 2*320, 240);
    osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);
    
    j = 0;
    for (i=0;i<(320*240);i++) {
        screenDirect[i] = graphicsRGB2PackedA(imBuff[j++], imBuff[j++], imBuff[j++]);
    }

    while(1) {
        while(!((c=buttonsGetStatusA()) & BUTTONS_AV300_ANY)) {}
        if (c&BUTTONS_AV300_OFF) break;
        if (c&BUTTONS_AV300_MENU1) {
            vmode++;
            if (vmode==3) vmode=0;
            if (vmode==0) {
                gioSetBitA(GIO_LINE_EXT_VIDEO);
                gioClearBitA(GIO_LINE_BACKLIGHT_POWER);
                osdSetVideoModeA(0x4071);
                osdSetLCDOnA();
                osdSetMainShiftA(0x79, 0x00);
                osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x13);
                osdSetComponentSizeA(OSD_VIDEO1, 2*320, 240);
            } else if (vmode==1) {
                gioClearBitA(GIO_LINE_EXT_VIDEO);
                gioClearBitA(GIO_LINE_BACKLIGHT_POWER);
                osdSetVideoModeA(0x407d | OSD_VIDEOMODE_NTSC);
                osdSetLCDOffA();
                osdSetMainShiftA(0x78, 0x12);
                osdSetComponentPositionA(OSD_VIDEO1, 0x3a, 0x18);
                osdSetComponentSizeA(OSD_VIDEO1, 2*300, 192);
            } else if (vmode==2) {
                gioClearBitA(GIO_LINE_EXT_VIDEO);
                gioSetBitA(GIO_LINE_BACKLIGHT_POWER);
                osdSetVideoModeA(0x407d | OSD_VIDEOMODE_PAL);
                osdSetLCDOffA();
                osdSetMainShiftA(0xae, 0x2d);
                osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x00);
                osdSetComponentSizeA(OSD_VIDEO1, 2*300, 240);
            }
            while((buttonsGetStatusA() & BUTTONS_AV300_MENU1)) {}
        }
    }
}

