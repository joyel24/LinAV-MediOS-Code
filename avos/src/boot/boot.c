#include "string.h"

#include <ata.h>
#include <fat.h>
#include <uart.h>
#include <file.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <usb.h>
#include <rtc.h>
#include <system.h>
#include <uart.h>
#include <debug.h>
#include <mathASM.h>

#include "bgimage.h"

int launchFile(char * fileN);
int pluginFile(char * fileN, char * ext);
int exeFile(char * fileN, char * arg);

static int pal32[2] = {0x6c706c93, 0xffffffff};
static int pal16[2] = {0x0000, 0xffff};
static int pal16b[5] = {0x1717, 0x3131, 0x0000, 0x1414, 0xcece};

static struct graphicsBuffer screenBitmap2;
static struct graphicsBuffer sprite5_7 = {0, 1, 5, 7, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
static struct graphicsBuffer sprite6_9 = {0, 1, 6, 9, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
static struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};

static unsigned int scrollbar[15][11] = {
 {0x800380,0x800980,0x800080,0x800a80,0x800380,0x800c80,0x800580,0x800080,0x800080,0x800a80,0x800080},
 {0x80dc80,0x80e380,0x80d980,0x7fe080,0x7fd580,0x80de80,0x80de80,0x80e380,0x80e080,0x80e580,0x80c280},
 {0x80de80,0x80de80,0x80d480,0x80de80,0x80d780,0x80e580,0x80de80,0x80e580,0x80db80,0x80dc80,0x80b580},
 {0x80e380,0x7fe080,0x80de80,0x80e780,0x80dc80,0x80e580,0x7fd580,0x80dc80,0x80dc80,0x7fe080,0x80c080},
 {0x80d980,0x80e780,0x80d680,0x807d80,0x800380,0x800080,0x802580,0x808a80,0x80db80,0x80de80,0x80bd80},
 {0x80e580,0x80da80,0x80e580,0x80ea80,0x80de80,0x80e980,0x7fc980,0x80d980,0x7fe080,0x7fe080,0x80b980},
 {0x80db80,0x80de80,0x80d780,0x807e80,0x800780,0x800080,0x801980,0x808280,0x80de80,0x80de80,0x80b580},
 {0x80e280,0x80d280,0x80e280,0x80e380,0x80d780,0x80e780,0x80c980,0x80e580,0x80dd80,0x80de80,0x80b980},
 {0x80d980,0x80e980,0x80d780,0x809280,0x800080,0x800080,0x801480,0x808b80,0x80de80,0x80d780,0x80c080},
 {0x80ea80,0x80d480,0x80e780,0x80dd80,0x80dc80,0x7fe080,0x80cb80,0x80de80,0x80de80,0x80d680,0x80bd80},
 {0x80cd80,0x80e980,0x80d280,0x809680,0x800080,0x800080,0x801180,0x808680,0x80e380,0x80dc80,0x80c280},
 {0x7fed80,0x80d780,0x80da80,0x80e080,0x80e880,0x80f080,0x7fd580,0x80d280,0x80e380,0x80dd80,0x80c280},
 {0x80d780,0x80de80,0x80ee80,0x80e780,0x80db80,0x80d680,0x80dc80,0x80ee80,0x80de80,0x80da80,0x80c480},
 {0x80bf80,0x80c280,0x80aa80,0x80c080,0x80a780,0x80c280,0x80b180,0x80bd80,0x80c080,0x80bd80,0x80ad80},
 {0x800080,0x800080,0x800e80,0x800080,0x801380,0x800080,0x800a80,0x800080,0x800780,0x800a80,0x800080},
};
static struct graphicsBuffer scrollBarB = {(u32) scrollbar, 11*4, 11, 15, 32, 5, -1, 0, 0, 0, 0, 0, 0};

struct dispDir {
	char name[14];
	char filename[14];
	int attr;
	char ext[4];
};

static struct tm* ourTime;
static char timeSt[] = "xx:xx:xx";
static char powerSt[] = "xxxx+";

static int source = 0;             // 0 = HDD, 1 = memCard
static int mode = 0;               // 0 = normal, 1 = usb

static char callArg1[MAX_PATH];
static char callArg2[MAX_PATH];
static char * callArgs[2] = {callArg1, callArg2};

static struct dispDir dirBufferHDD[1000];

static char nameCurHDD[MAX_PATH] = "/";

static unsigned int * screenDirect = 0x03a00000;

int main() {
    unsigned int a, a1, a2;
    int c, b, i, totalEntries;
    int cursorposHDD=0;
    int dirposHDD=0;
    
    int loopDelay = 0xc000;
    int cursorMoved=1;
    void (*systemRelocateAdjusted)();

    systemRelocateAdjusted = systemRelocateMeA - 0x00400000;    
    systemRelocateAdjusted();

    
startInit:
	inifile();
	inidir();
	inifatinfo();

    cursorposHDD = 0;
    dirposHDD = 0;
    loopDelay = 0xc000;
                            // Preserve source and dir...
    mode = 0;
    cursorMoved = 1;

    osdInitA();
    rtcInit();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);
    
    screenBitmap2.offset = 0x03a00000;
    screenBitmap2.bytesPerLine = 320*4;
    screenBitmap2.width = 320;
    screenBitmap2.height =240;
    screenBitmap2.bitsPerPixelShift = 5;
    screenBitmap2.bitsPerPixel = 32;

    //graphicsSpriteA(&screenBitmap2, 0, 0, &bgimageB);

    for (c=0;c<240;c++) {
        for (i=0;i<320;i++) {
            a = ((int)bgimage[c][i*3]) | ((int)bgimage[c][i*3 + 1]<<8) | ((int)bgimage[c][i*3 + 2]<<16);
            screenDirect[c*320 + i] = a;
        }
    }
    
    osdSetComponentSizeA(OSD_VIDEO1, 320*2, 240);
    osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03a00000);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);

	// EUGH: TODO - Clean this rubbish up!
    usbDisableA();
    for (c=0;c<0x14000;c++) {}
//    ataPowerDownHDD();
    for (c=0;c<0x14000;c++) {}
    ataSelectMemoryCardA();

    for (c=0;c<0x14000;c++) {}
    ataSelectHDDA();
    for (c=0;c<0x14000;c++) {}
    usbEnableA();
    for (c=0;c<0x14000;c++) {}
    ataPowerUpHDDA();
    for (c=0;c<0x14000;c++) {}
    usbDisableA();
    for (c=0;c<0x24000;c++) {}

	ataReadMBR();

	int fatHD=-1;
    int fatCF=-1;

	for(i=0;i<4;i++) {
        debug("Partition %d\n", i);
        printPartInfo(i);
    }

    fatHD = fatInit(getPartition(0));
	debug("[boot.c] fatInit returned = %d\n",fatHD);

	int dir;
	struct dirent* entry;

    while(1) {
        if (mode==0) {
            graphicsBoxfA(&screenBitmap2, 21, 52, 96, 152, 0x466c4696);    

            debug("Listing contents of '%s'\n", nameCurHDD);
            if((dir=opendir(nameCurHDD))<0)
			{
				uartOutsA("dir not found trying root...\n");
                nameCurHDD[0] = '/';
                nameCurHDD[1] = 0;
                if ((dir=opendir(nameCurHDD))<0) break;
			}

			int i=0;
			int len;
			while((entry=readdir(dir))!=NULL && i<100)
			{
                strcpy(dirBufferHDD[i].name, entry->entryName);

				for(len=strlen(dirBufferHDD[i].name);len<12;len++)
					dirBufferHDD[i].name[len]=' '; 
				dirBufferHDD[i].name[12]=0x0;

				strcpy(dirBufferHDD[i].filename, entry->entryName);
				strcpy(dirBufferHDD[i].ext, entry->ext);
				dirBufferHDD[i].attr=entry->attribute;
				i++;
			}
			closedir(dir);
			totalEntries=i;

            debug("dir entries = %d\n", i);
            
            cursorposHDD=0;
            dirposHDD=0;
        }
            
        while(1) {
            c = usbIsConnectedA();
            pal32[0] = 0x466c4696;
            pal32[1] = 0xff80ff80;
            if (c) {
                graphicsStringA(&screenBitmap2, 2, 2, &sprite8_13, std8x13_, 8, 0,
                    "[USB]");
            } else {
                graphicsStringA(&screenBitmap2, 2, 2, &sprite8_13, std8x13_, 8, 0,
                    "     ");
            }

            if (mode!=0) {
                graphicsStringA(&screenBitmap2, 2 + 6*8, 2, &sprite8_13, std8x13_, 8, 0,
                    "[USB mode]");
                
            } else {
                graphicsStringA(&screenBitmap2, 2 + 6*8, 2, &sprite8_13, std8x13_, 8, 0,
                    "          ");
            }

            pal32[0] = 0x6c706c93;
            if (source==0) {
                graphicsStringA(&screenBitmap2, 2 + 19*8, 2, &sprite8_13, std8x13_, 8, 0,
                    "[HDD]");
                
            } else {
                graphicsStringA(&screenBitmap2, 2 + 19*8, 2, &sprite8_13, std8x13_, 8, 0,
                    "[MEM]");
            }

            b = powerGetStatusA();
            stringPutHexA(powerSt, b, 4);

            if (powerIsDCConnectedA()) {
                powerSt[4] = '+';                
            } else {
                powerSt[4] = ' ';
            }
            graphicsStringA(&screenBitmap2, 2 + 25*8, 2, &sprite8_13, std8x13_, 8, 0,
                        powerSt);
            
            ourTime = rtcGetTime();
            stringPutHexA(timeSt+6, ourTime->tm_sec, 2);
            stringPutHexA(timeSt+3, ourTime->tm_min, 2);
            stringPutHexA(timeSt, ourTime->tm_hour, 2);
            graphicsStringA(&screenBitmap2, 320-(8*9)-2, 2, &sprite8_13, std8x13_, 9, 0,
                        timeSt);

            b = powerGetStatusA();
            stringPutHexA(powerSt, b, 4);

            // Update file display window if needed,
            
            if (mode==0 && cursorMoved==1) {
                // Update scrollbar (Now on VIDEO plane)...
                graphicsBoxfA(&screenBitmap2, 124, 57, 11, 141, 0x80808080);   

// 134, 197
// 124, 57
                
                a = (dirposHDD+cursorposHDD) * (140-15);
                if (totalEntries>1) a = mathDivLUA(0, a, totalEntries-1);
                graphicsSpriteA(&screenBitmap2, 124, 57 + a, &scrollBarB);


                for (c=0;c<16;c++) {
                    if ((dirposHDD+c)>=totalEntries) break;

                    pal32[1] = 0xff80ff80;
                    if (dirBufferHDD[dirposHDD+c].attr & FAT_ATTR_DIR)
                        pal32[1] = 0xc476c491;
                    if (c==cursorposHDD) {
                        pal32[0] = 0x00800080;
                    } else {
                        pal32[0] = 0x466c4696;
                    }
    
                    graphicsStringA(&screenBitmap2, 21, 52 + c*9, &sprite6_9, std6x9_, 6, 0,
                        dirBufferHDD[dirposHDD+c].name);
                }

				pal32[1] = 0xff80ff80;
				pal32[0] = 0x466c4696;

				for(;c<16;c++)
					graphicsStringA(&screenBitmap2, 21, 52 + c*9, &sprite6_9, std6x9_, 6, 0,
                        "            ");

                cursorMoved=0;
            }
                
                
            c =buttonsGetStatusA();
            if (!(c & BUTTONS_AV300_ANY)) loopDelay = 0xc000;

            if (mode==0) {
                if (c & BUTTONS_AV300_DOWN) {
                    if ((cursorposHDD<15) && (cursorposHDD<(totalEntries-1))) {
                        cursorposHDD++;
                    } else {
                        if ((dirposHDD+15)<(totalEntries-1)) dirposHDD++;
                    }
                    cursorMoved=1;
                    if (loopDelay>=0x1000) loopDelay-=0x1000;
                } else if (c & BUTTONS_AV300_UP) {
                    if (cursorposHDD>0) {
                        cursorposHDD--;
                    } else {
                        if (dirposHDD>0) dirposHDD--;
                    }
                    if (loopDelay>=0x1000) loopDelay-=0x1000;
                    cursorMoved=1;
                } else if (c & BUTTONS_AV300_LEFT) {
					if(strcmp(nameCurHDD,"/")!=0)	{
						char * namePos=strrchr(nameCurHDD+1,'/');
						if ( namePos )
							*namePos = 0;
						namePos=strrchr(nameCurHDD,'/');
						if ( namePos )
							*(namePos+1) = 0;
						cursorMoved=1;
						do {
						} while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
						break;
                    }
                } else if (c & BUTTONS_AV300_RIGHT) {
                    if (dirBufferHDD[dirposHDD+cursorposHDD].attr & FAT_ATTR_DIR) {
                        if(strcmp(dirBufferHDD[dirposHDD+cursorposHDD].filename,"..")==0)
						{
							char * namePos=strrchr(nameCurHDD+1,'/');
							if ( namePos )
							{
								*namePos = 0;
							}
							namePos=strrchr(nameCurHDD,'/');
							if ( namePos )
							{
								*(namePos+1) = 0;
							}

						}
						else if (strcmp(dirBufferHDD[dirposHDD+cursorposHDD].filename,".")!=0)
						{
							strcat(nameCurHDD, dirBufferHDD[dirposHDD+cursorposHDD].filename);
							strcat(nameCurHDD, "/");
						}
                        cursorMoved=1;
                        do {
                        } while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
                        break;
                    }
                } else if (c & BUTTONS_AV300_OFF) {
                    for (a=0;a<20000;a++) {
                        if (!(buttonsGetStatusA() & BUTTONS_AV300_ANY)) {
                            launchFile("/DEFAULT.BIN");
                            goto startInit;                            
                        }
                    }
                    do {
                    } while(buttonsGetStatusA() & BUTTONS_AV300_ANY);                    
                } else if (c & BUTTONS_AV300_ON) {
                    if (dirBufferHDD[dirposHDD+cursorposHDD].attr & FAT_ATTR_DIR) {
                        if(strcmp(dirBufferHDD[dirposHDD+cursorposHDD].filename,"..")==0)
						{
							char * namePos=strrchr(nameCurHDD+1,'/');
							if ( namePos )
							{
								*namePos = 0;
							}
							namePos=strrchr(nameCurHDD,'/');
							if ( namePos )
							{
								*(namePos+1) = 0;
							}

						}
						else if (strcmp(dirBufferHDD[dirposHDD+cursorposHDD].filename,".")!=0)
						{
							strcat(nameCurHDD,dirBufferHDD[dirposHDD+cursorposHDD].filename);
							strcat(nameCurHDD,"/");
						}
                        cursorMoved=1;
                        do {
                        } while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
                        break;
                    } else {
                        char fileN[MAX_PATH];
						strcpy(fileN, nameCurHDD);
						strcat(fileN, dirBufferHDD[dirposHDD+cursorposHDD].filename);

                        if (strcmp(dirBufferHDD[dirposHDD+cursorposHDD].ext, "BIN")==0) {
                            // Launch it...
                            launchFile(fileN);
                            goto startInit;
                        } else {
                            // Try for a plugin...   
                            if (pluginFile(fileN, dirBufferHDD[dirposHDD+cursorposHDD].ext))
                                goto startInit;
                        }
                    }
                    do {
                    } while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
                }
            }
            
            // Other non mode 0 specific stuff...

            if (c & BUTTONS_AV300_MENU3) {
                if (source==0) {
                    mode ^= 1;
                    do {
                    } while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
                    cursorMoved=1;
                    if (mode==0) {
                        usbDisableA();
                    } else {
                        usbEnableA();
                    }
    
                    if (source==0) {
                        ataPowerUpHDDA();
                        ataSelectHDDA();
                    } else {
                        ataPowerDownHDDA();
                        ataSelectMemoryCardA();
                    }
    
                    // TODO - reinit? USB could have modified disk...
                    // Only matters if dir struct changed etc
                    
                    break;
                }
            } else if (c & BUTTONS_AV300_MENU2) {
                if (mode==0) {
                    a1 = 0;
                    if (source==0) {
                        a1 = gioGetBitA(0x15) + gioGetBitA(0x16);
                        debug("GIO 15+16 = %d", a1);
                    }

                    if (a1==0) {
                        source ^= 1;    
                        
                        if (source==0) {
                            ataPowerUpHDDA();
                            ataSelectHDDA();
                        } else {
                            ataPowerDownHDDA();
                            ataSelectMemoryCardA();
                        }
                        
                        ataReadMBR();
    
                        for(i=0;i<4;i++)
                            printPartInfo(i);
    
                        closeFat(fatHD);
     
                        fatHD = fatInit(getPartition(0));
                        debug("[fatTest.c] fatInit returned = %d\n",fatHD);
                        selectFat(fatHD);
        
                        nameCurHDD[0]='/';
                        nameCurHDD[1]=0;
        
                        do {
                        } while(buttonsGetStatusA() & BUTTONS_AV300_ANY);
                        cursorMoved=1;
                        break;
                    }
                }
            }
            for (i=0;i<loopDelay;i++) {}          // Little delay...
        }
    }
    
    debug("All done!");

	if(fatHD!=-1)
		closeFat(fatHD);

	if(fatCF!=-1)
		closeFat(fatCF);

    while(1) {}
}

int launchFile(char * fileN) {
    debug("[launchFile] '%s'\n", fileN);
	return exeFile(fileN, NULL);
}

int pluginFile(char * fileN, char * ext) {
    char lname[] = "/PLUGINS/xxx.BIN";
    debug("[pluginFile] '%s'\n", fileN);
    lname[9] = ext[0];
    lname[10] = ext[1];
    lname[11] = ext[2];
	if(!exeFile(lname, fileN)) {
		debug("[pluginFile] error launching plugin %s\n", fileN);
        return 0;
    }
	return 1;
}

// general launching function

int exeFile(char * fileN, char * arg)
{
    void (*codeCaller)(int argc, char * argv[])=(void (*)(int argc, char * argv[]))0x03000000;
    strcpy(callArg1, fileN);
    debug("exeFile '%s','%s','%s'\n", fileN, arg, callArg1);
   
	if(loadFile(callArg1)) {
		int argc=1;

		if(arg!=NULL) {
            strcpy(callArg2, arg);
			argc++;
		}

        debug("Calling file now...\n");
		codeCaller(argc, callArgs);
		return 1;
	} else {
		return 0;
    }
}


