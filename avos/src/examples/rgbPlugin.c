#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <ata.h>
#include <file.h>
#include <debug.h>

int colorRGB2Packed(int r, int g, int b);

struct graphicsBuffer screenVideo;

static char imBuff[320*240*3];
static unsigned int * screenDirect = 0x03c00000;

int main(int argc, char * * argv) {
    int i, j, r, g, b, v;
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

    osdSetComponentSizeA(OSD_VIDEO1, 640, 240);
    osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);    

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

    j = 0;
    for (i=0;i<(320*240);i++) {
        screenDirect[i] = graphicsRGB2PackedA(imBuff[j++], imBuff[j++], imBuff[j++]);
    }

    ataPowerDownHDDA();
    
    while(!(buttonsGetStatusA() & BUTTONS_AV300_ANY)) {}
}

