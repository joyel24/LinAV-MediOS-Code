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
    
char parentName[] = "..         ";
char ext_bin[] = "BIN";



int main() {
    int c, b, i, totalEntries;
    int cursorpos=0;
    int dirpos=0;
    unsigned int cluster=0, pcluster=0, parent=0;
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
    parent = 0;
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

    graphicsBoxf(&screenBitmap, 0, 0, 96, 132, 0xd5d5);    
    graphicsBoxf(&screenBitmap, 1, 1, 94, 130, 0x1414);    

    screenBitmap2.offset = 0x03b00000;
    screenBitmap2.bytesPerLine = 320*2;
    screenBitmap2.width = 320;
    screenBitmap2.height =240;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap2, 0, 0, 320, 240, 0x0000);
    graphicsBoxf(&screenBitmap2, 0, 0, 320, 10, 0xd5d5);
    graphicsBoxf(&screenBitmap2, 0, 11, 320, 1, 0x1717);
    graphicsBoxf(&screenBitmap2, 0, 228, 320, 1, 0x1717);
    graphicsBoxf(&screenBitmap2, 0, 229, 320, 10, 0xd5d5);
    pal16[0] = 0xd5d5;
    pal16[1] = 0xffff;    
    graphicsString(&screenBitmap2, 4, 231, &sprite5_7, std5x7_, 6, 0,
                        "[Menu2] HDD/Memcard   [Menu3] UsbMode   [ON] Click");

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


    // EUGH: TODO - Clean this rubbish up!
    usbDisable();
    for (c=0;c<0x14000;c++) {}
//    ataPowerDownHDD();
    for (c=0;c<0x14000;c++) {}
    ataSelectMemoryCard();

    for (c=0;c<0x14000;c++) {}
    ataSelectHDD();
    for (c=0;c<0x14000;c++) {}
    usbEnable();
    for (c=0;c<0x14000;c++) {}
    ataPowerUpHDD();
    for (c=0;c<0x14000;c++) {}
    usbDisable();
    for (c=0;c<0x24000;c++) {}

    c = ataReadSectors(0, 1, mbr);
    stringPutHex(hex82, c, 8);
    uartOuts("[fatTest.c] MBR read returned = ");
    uartOuts(hex82);
    uartOuts("\n");

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

            c = fatReadFile(cluster, (char*) dirBuffer);
            stringPutHex(hex82, c, 8);
            uartOuts("[fatTest.c] fatReadFile returned = ");
            uartOuts(hex82);
            uartOuts("\n");

            totalEntries = fatDirFilter(dirBuffer, dirBuffer2, 1000);
        
            parent = cluster;       // Assume no parent...
            
            for (c=0;c<totalEntries;c++) {
                i = 1;              // Assume it matches...
                for (b=0;b<11;b++) {
                    if (dirBuffer2[c].name[b]!=parentName[b]) {
                        i = 0;
                        break;
                    }
                }
                // i = 1 if we found the parent...
                if (i==1) {
                    parent = dirBuffer2[c].fatCluHI << 16
                            | dirBuffer2[c].fatCluLO;
                    if (parent==0) parent = getRootClu();
                    break;
                }
            }

            //showBuffer((char*) dirBuffer2);
        
            cursorpos=0;
            dirpos=0;
            osdSetComponentConfig(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A6
                                     | OSD_BITMAP_8BIT);
        } else {
            osdSetComponentConfig(OSD_BITMAP2, 0);
        }
            
        while(1) {

            c = usbIsConnected();
            pal16[0] = 0xd5d5;            
            pal16[1] = 0xffff;
            if (c) {
                graphicsString(&screenBitmap2, 4, 2, &sprite5_7, std5x7_, 6, 0,
                    "[USB Lead in]");
                
            } else {
                graphicsString(&screenBitmap2, 4, 2, &sprite5_7, std5x7_, 6, 0,
                    "             ");
            }

            if (mode!=0) {
                graphicsString(&screenBitmap2, 4 + 14*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "[USB mode]");
                
            } else {
                graphicsString(&screenBitmap2, 4 + 14*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "          ");
            }

            if (source==0) {
                graphicsString(&screenBitmap2, 4 + 25*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "[HDD]    ");
                
            } else {
                graphicsString(&screenBitmap2, 4 + 25*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "[MemCard]");
            }
            
            ourTime = rtcGetTime();
            stringPutHex(timeSt+9, ourTime->tm_ms, 2);
            stringPutHex(timeSt+6, ourTime->tm_sec, 2);
            stringPutHex(timeSt+3, ourTime->tm_min, 2);
            stringPutHex(timeSt, ourTime->tm_hour, 2);
            pal16[1] = 0xffff;
            graphicsString(&screenBitmap2, 4 + 41*6, 2, &sprite5_7, std5x7_, 6, 0,
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
                } else if (c & BUTTONS_AV300_LEFT) {
                    if (cluster!=parent) {      // No point then!
                        cluster = parent;
                        cursorMoved=1;
                        do {
                            c =buttonsGetStatus();
                        } while(c & BUTTONS_AV300_ANY);                    
                        break;
                    }
                } else if (c & BUTTONS_AV300_RIGHT) {
                    if (dirBuffer2[dirpos+cursorpos].attr & FAT_ATTR_DIR) {
                        cluster = dirBuffer2[dirpos+cursorpos].fatCluHI << 16
                            | dirBuffer2[dirpos+cursorpos].fatCluLO;
                        if (cluster==0) cluster=getRootClu();
                        cursorMoved=1;
                        do {
                            c =buttonsGetStatus();
                        } while(c & BUTTONS_AV300_ANY);                    
                        break;
                    }
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
                        i = 1;                  // Assume match
                        for (c=0;c<3;c++) {
                            if (dirBuffer2[dirpos+cursorpos].ext[c] != ext_bin[c]) {
                                i=0;
                                break;
                            }
                        }
                        
                        if (i==1) {
                            
                            c = fatReadFile(pcluster, (char*) 0x03000000);
                            stringPutHex(hex82, c, 8);
                            uartOuts("[fatTest.c] fatReadFile returned = ");
                            uartOuts(hex82);
                            uartOuts("\n");
                            
                            // Now CALL IT!
                            
                            codeCaller();           // Go go go!
    
                            goto startInit;
                            
                        } else {
                            // HANDLE OTHER PLUGINS?    
                        }
                    }
                    do {
                        c =buttonsGetStatus();
                    } while(c & BUTTONS_AV300_ANY);
                }
            }
            
            // Other non mode 0 specific stuff...
            
            if (c & BUTTONS_AV300_MENU3) {
                mode ^= 1;
                do {
                    c =buttonsGetStatus();
                } while(c & BUTTONS_AV300_ANY);
                cursorMoved=1;
                if (mode==0) {
                    usbDisable();
                } else {
                    usbEnable();    
                }

                if (source==0) {
                    ataPowerUpHDD();
                    ataSelectHDD();
                } else {
                    ataPowerDownHDD();
                    ataSelectMemoryCard();
                }
                
                // TODO - reinit? USB could have modified disk...
                
                break;
            } else if (c & BUTTONS_AV300_MENU2) {
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
