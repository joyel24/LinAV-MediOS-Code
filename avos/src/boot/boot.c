#include <ata.h>
#include <fat.h>
#include <uart.h>
#include <string.h> 
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <usb.h>
#include <rtc.h>
#include <system.h>

void showBuffer(char *source);

static int pal32[2] = {0x8080c0e0, 0xffffffff};
static int pal16[2] = {0x0000, 0xffff};

struct graphicsBuffer screenBitmap;
struct graphicsBuffer screenBitmap2;
struct graphicsBuffer sprite5_7 = {0, 1, 5, 7, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
struct graphicsBuffer spriteShadow = {0, 2, 12, 18, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};

struct dirEntry dirBuffer[10000];
struct dirEntry dirBuffer2[10000];
unsigned char mbr[512];

char hex82[] = "xxxxxxxx";

char dirLine[] = "xxxxxxxx.xxx\n";

struct tm* ourTime;
    char timeSt[] = "xx:xx:xx.xx";
    
int main() {
    int c, i, totalEntries;
    int cursorpos=0;
    int dirpos=0;
    unsigned int cluster=0, pcluster=0;
    int loopDelay = 0xc000;
    int source = 0;             // 0 = HDD, 1 = memCard
    int mode = 0;               // 0 = normal, 1 = usb
    int part;
    int cursorMoved=1;
    void (*codeCaller)();
    
    codeCaller = (void (*)())0x03000000;

    void (*systemRelocateAdjusted)();
    systemRelocateAdjusted = systemRelocateMe - 0x00400000;    
    systemRelocateAdjusted();

startInit:
    cursorpos = 0;
    dirpos = 0;
    cluster = 0;
    loopDelay = 0xc000;
                            // Preserve source
    mode = 0;
    cursorMoved = 1;

    osdInit();

    rtcInit();
    
    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03e00000;
    screenBitmap.bytesPerLine = 96*2;
    screenBitmap.width = 96;
    screenBitmap.height = 132;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap, 0, 0, 96, 132, 0x0202);    
    graphicsBoxf(&screenBitmap, 1, 1, 94, 130, 0x1414);    

    screenBitmap2.offset = 0x03b00000;
    screenBitmap2.bytesPerLine = 320*2;
    screenBitmap2.width = 320;
    screenBitmap2.height =240;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap2, 0, 0, 320, 240, 0x0000);
    pal16[0] = 0x0000;
    pal16[1] = 0xffff;    
    graphicsString(&screenBitmap2, 2, 231, &sprite4_6, std4x6_, 5, 0,
                        "[Menu1] HDD/Memcard   [OFF] UsbMode   [ON] Click");

    osdSetComponentSize(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPosition(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffset(OSD_BITMAP1, 0x03b00000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x14);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);
    osdSetComponentSize(OSD_BITMAP2, 96*2, 132);
    osdSetComponentPosition(OSD_BITMAP2, 0x14 + (2*16), 0x12 + 32);
    osdSetComponentOffset(OSD_BITMAP2, 0x03e00000);
    osdSetComponentSourceWidth(OSD_BITMAP2, 6);
    osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A6
                                     | OSD_BITMAP_8BIT);

    ataPowerUpHDD();
    ataSelectHDD();

    ataReadSectors(0, 1, mbr);
    
    showBuffer(mbr);
    
    part = mbr[0x1c6] | (mbr[0x1c7]<<8) | (mbr[0x1c8]<<16) | (mbr[0x1c9]<<24);
    stringPutHex(hex82, part, 8);
    uartOuts("[fatTest.c] Partition1 = ");
    uartOuts(hex82);
    uartOuts("\n");
    if (part==0) part=0x3f;
    
    c = fatInit(part);
    stringPutHex(hex82, c, 8);
    uartOuts("[fatTest.c] fatInit returned = ");
    uartOuts(hex82);
    uartOuts("\n");
    
    cluster = getRootClu();

    while(1) {

        if (mode==0) {
            graphicsBoxf(&screenBitmap, 1, 1, 94, 130, 0x1414);    

            for (c=0;c<1000;c++) {
                dirBuffer[c].name[0] = 0;    
            }

            usbDisable();
        
            c = fatReadFile(cluster, (char*) dirBuffer);
            stringPutHex(hex82, c, 8);
            uartOuts("[fatTest.c] fatReadFile returned = ");
            uartOuts(hex82);
            uartOuts("\n");

            totalEntries = fatDirFilter(dirBuffer, dirBuffer2, 1000);
        
            //showBuffer((char*) dirBuffer2);
        
            cursorpos=0;
            dirpos=0;
            osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A6
                                     | OSD_BITMAP_8BIT);
        } else {
            osdSetComponentConfig(OSD_BITMAP2, 0);
            usbEnable();
        }
            
        while(1) {

            c = usbIsConnected();
            pal16[0] = 0x0000;            
            pal16[1] = 0x4e4e;
            if (c) {
                graphicsString(&screenBitmap2, 2, 2, &sprite4_6, std4x6_, 5, 0,
                    "[USB Lead in]");
                
            } else {
                graphicsString(&screenBitmap2, 2, 2, &sprite4_6, std4x6_, 5, 0,
                    "             ");
            }

            if (mode!=0) {
                graphicsString(&screenBitmap2, 2 + 14*5, 2, &sprite4_6, std4x6_, 5, 0,
                    "[USB mode]");
                
            } else {
                graphicsString(&screenBitmap2, 2 + 14*5, 2, &sprite4_6, std4x6_, 5, 0,
                    "          ");
            }

            if (source==0) {
                graphicsString(&screenBitmap2, 2 + 25*5, 2, &sprite4_6, std4x6_, 5, 0,
                    "[HDD]    ");
                
            } else {
                graphicsString(&screenBitmap2, 2 + 25*5, 2, &sprite4_6, std4x6_, 5, 0,
                    "[MemCard]");
            }
            
            ourTime = rtcGetTime();
            stringPutHex(timeSt+9, ourTime->tm_ms, 2);
            stringPutHex(timeSt+6, ourTime->tm_sec, 2);
            stringPutHex(timeSt+3, ourTime->tm_min, 2);
            stringPutHex(timeSt, ourTime->tm_hour, 2);
            pal16[1] = 0xffff;
            graphicsString(&screenBitmap2, 2 + 35*5, 2, &sprite4_6, std4x6_, 5, 0,
                        timeSt);

            // Update file display window if needed,
            
            if (mode==0 && cursorMoved==1) {
                for (c=0;c<16;c++) {
                    if ((dirpos+c)>=totalEntries) break;
    
                    for (i=0;i<8;i++) dirLine[i] = dirBuffer2[dirpos+c].name[i];
                    for (i=0;i<3;i++) dirLine[i+9] = dirBuffer2[dirpos+c].ext[i];
    //                uartOuts(dirLine);
    
                    pal16[1] = 0xffff;
                    if (dirBuffer2[dirpos+c].attr & FAT_ATTR_DIR)
                        pal16[1] = 0x4e4e;
                    if (c==cursorpos) {
                        pal16[0] = 0x0000;
                    } else {
                        pal16[0] = 0x1414;            
                    }
    
                    graphicsString(&screenBitmap, 3, 3 + c*8, &sprite5_7, std5x7_, 5, 0,
                        dirLine);
                }
                cursorMoved=0;
            }
                
                
            c =buttonsGetStatus();
            if (!(c & BUTTONS_AV300_ANY)) loopDelay = 0xc000;

            if (mode==0) {
                if (c & BUTTONS_AV300_DOWN) {
                    if ((cursorpos<15) && (cursorpos<(totalEntries-1))) {
                        cursorpos++;
                    } else {
                        if ((dirpos+15)<(totalEntries-1)) dirpos++;
                    }
                    cursorMoved=1;
                    if (loopDelay>=0x1000) loopDelay-=0x1000;
                } else if (c & BUTTONS_AV300_UP) {
                    if (cursorpos>0) {
                        cursorpos--;
                    } else {
                        if (dirpos>0) dirpos--;
                    }
                    if (loopDelay>=0x1000) loopDelay-=0x1000;
                    cursorMoved=1;
                } else if (c & BUTTONS_AV300_ON) {
                    if (dirBuffer2[dirpos+cursorpos].attr & FAT_ATTR_DIR) {
                        cluster = dirBuffer2[dirpos+cursorpos].fatCluHI << 16
                            | dirBuffer2[dirpos+cursorpos].fatCluLO;
                        if (cluster==0) cluster=getRootClu();
                        cursorMoved=1;
                        do {
                            c =buttonsGetStatus();
                        } while(c & BUTTONS_AV300_ANY);                    
                        break;
                    } else {
                        pcluster = dirBuffer2[dirpos+cursorpos].fatCluHI << 16
                            | dirBuffer2[dirpos+cursorpos].fatCluLO;
                        // TODO
                        //  Handle plugins
                        //  text file reader
                        //  image reader
                        //  mp3 player
                        //  etc

                        // for now, RUN IT!
                        
                        c = fatReadFile(pcluster, (char*) 0x03000000);
                        stringPutHex(hex82, c, 8);
                        uartOuts("[fatTest.c] fatReadFile returned = ");
                        uartOuts(hex82);
                        uartOuts("\n");
                        
                        // Now CALL IT!
                        
                        codeCaller();           // Go go go!

                        goto startInit;
                    }
                    do {
                        c =buttonsGetStatus();
                    } while(c & BUTTONS_AV300_ANY);
                }
            }
            
            // Other non mode 0 specific stuff...
            
            if (c & BUTTONS_AV300_OFF) {
                mode ^= 1;
                do {
                    c =buttonsGetStatus();
                } while(c & BUTTONS_AV300_ANY);
                cursorMoved=1;
                break;
            } else if (c & BUTTONS_AV300_MENU1) {
                source ^= 1;    
                if (source==0) {
                    ataPowerUpHDD();
                    ataSelectHDD();
                } else {
                    ataPowerDownHDD();
                    ataSelectMemoryCard();
                }

                ataReadSectors(0, 1, mbr);
                part = mbr[0x1c6] | (mbr[0x1c7]<<8) | (mbr[0x1c8]<<16) | (mbr[0x1c9]<<24);
                stringPutHex(hex82, part, 8);
                uartOuts("[fatTest.c] Partition1 = ");
                uartOuts(hex82);
                uartOuts("\n");
                if (part==0) part=0x3f;

                c = fatInit(part);
                stringPutHex(hex82, c, 8);
                uartOuts("[fatTest.c] fatInit returned = ");
                uartOuts(hex82);
                uartOuts("\n");
    
                cluster = getRootClu();
                
                do {
                    c =buttonsGetStatus();
                } while(c & BUTTONS_AV300_ANY);
                cursorMoved=1;
                break;
            }
            for (i=0;i<loopDelay;i++) {}          // Little delay...

        }
    }
    
    uartOuts("All done!");
    
    while(1) {}
}

char p[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxxxxxxxxx\n";

void showBuffer(char *source) {
    int i,j;
    char c;

    for (j=0;j<512;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHex(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
        uartOuts(p);
    }
}
