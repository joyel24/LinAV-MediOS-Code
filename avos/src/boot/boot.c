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
#include <power.h>
#include <uart.h>
#include <debug.h>

int launchFile(char * fileN);
void (*codeCaller)(int argc, char * argv[])=(void (*)(int argc, char * argv[]))0x03000000;

static int pal32[2] = {0x8080c0e0, 0xffffffff};
static int pal16[2] = {0x0000, 0xffff};

struct graphicsBuffer screenBitmap;
struct graphicsBuffer screenBitmap2;
struct graphicsBuffer sprite5_7 = {0, 1, 5, 7, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
struct graphicsBuffer spriteShadow = {0, 2, 12, 18, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};

struct dispDir {
	char name[13];
	int attr;
	char ext[4];
};

struct dispDir dirBuffer[10000];

	int (*action[])(char * name) ={launchFile};
	char * ext[]={"BIN"};
	int nbExt=1;

struct tm* ourTime;
char timeSt[] = "xx:xx:xx.xx";
    
char powerSt[] = "xxxx+";

int main() {
    int c, b, i, totalEntries;
    int cursorpos=0;
    int dirpos=0;
    unsigned int cluster=0, pcluster=0, parent=0;
    int loopDelay = 0xc000;
    int source = 0;             // 0 = HDD, 1 = memCard
    int mode = 0;               // 0 = normal, 1 = usb
    int cursorMoved=1;

    void (*systemRelocateAdjusted)();
    systemRelocateAdjusted = systemRelocateMeA - 0x00400000;    
    systemRelocateAdjusted();

startInit:
	inifile();
	inidir();

    cursorpos = 0;
    dirpos = 0;
    cluster = 0;
    parent = 0;
    loopDelay = 0xc000;
                            // Preserve source
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

    screenBitmap.offset = 0x03e00000;
    screenBitmap.bytesPerLine = 96*2;
    screenBitmap.width = 96;
    screenBitmap.height = 132;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, 96, 132, 0xd5d5);    
    graphicsBoxfA(&screenBitmap, 1, 1, 94, 130, 0x1414);    

    screenBitmap2.offset = 0x03b00000;
    screenBitmap2.bytesPerLine = 320*2;
    screenBitmap2.width = 320;
    screenBitmap2.height =240;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap2, 0, 0, 320, 240, 0x0000);
    graphicsBoxfA(&screenBitmap2, 0, 0, 320, 10, 0xd5d5);
    graphicsBoxfA(&screenBitmap2, 0, 11, 320, 1, 0x1717);
    graphicsBoxfA(&screenBitmap2, 0, 228, 320, 1, 0x1717);
    graphicsBoxfA(&screenBitmap2, 0, 229, 320, 10, 0xd5d5);
    pal16[0] = 0xd5d5;
    pal16[1] = 0xffff;    
    graphicsStringA(&screenBitmap2, 4, 231, &sprite5_7, std5x7_, 6, 0,
                        "[Menu2] HDD/Memcard   [Menu3] UsbMode   [ON] Click");

    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03b00000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);
    osdSetComponentSizeA(OSD_BITMAP2, 96*2, 132);
    osdSetComponentPositionA(OSD_BITMAP2, 0x14 + (2*16), 0x12 + 32);
    osdSetComponentOffsetA(OSD_BITMAP2, 0x03e00000);
    osdSetComponentSourceWidthA(OSD_BITMAP2, 6);
    osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A6
                                     | OSD_BITMAP_8BIT);


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

	for(i=0;i<4;i++)
		printPartInfo(i);

    c = fatInit(getPartition(0));
	debug("[fatTest.c] fatInit returned = %x\n",c);

	char nameCur[MAX_PATH]="/";
	int dir;
	struct dirent* entry;

    while(1) {
        if (mode==0) {
            graphicsBoxfA(&screenBitmap, 1, 1, 94, 130, 0x1414);    

            if((dir=opendir(nameCur))<0)
			{
				uartOutsA("dir not found\n");
				break;
			}

			int i=0;
			while((entry=readdir(dir))!=NULL && i<20)
			{
				strcpy(dirBuffer[i].name,entry->entryName);
				strcpy(dirBuffer[i].ext,entry->ext);
				dirBuffer[i].attr=entry->attribute;
				i++;
			}
			closedir(dir);
			totalEntries=i;

            cursorpos=0;
            dirpos=0;
            osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_MERGEBACK
                                     | OSD_BITMAP_A6
                                     | OSD_BITMAP_8BIT);
        } else {
            osdSetComponentConfigA(OSD_BITMAP2, 0);
        }
            
        while(1) {
            c = usbIsConnectedA();
            pal16[0] = 0xd5d5;            
            pal16[1] = 0xffff;
            if (c) {
                graphicsStringA(&screenBitmap2, 4, 2, &sprite5_7, std5x7_, 6, 0,
                    "[USB Lead in]");
                
            } else {
                graphicsStringA(&screenBitmap2, 4, 2, &sprite5_7, std5x7_, 6, 0,
                    "             ");
            }

            if (mode!=0) {
                graphicsStringA(&screenBitmap2, 4 + 14*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "[USB mode]");
                
            } else {
                graphicsStringA(&screenBitmap2, 4 + 14*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "          ");
            }

            if (source==0) {
                graphicsStringA(&screenBitmap2, 4 + 25*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "[HDD]    ");
                
            } else {
                graphicsStringA(&screenBitmap2, 4 + 25*6, 2, &sprite5_7, std5x7_, 6, 0,
                    "[MemCard]");
            }
            
            ourTime = rtcGetTime();
            stringPutHexA(timeSt+9, ourTime->tm_ms, 2);
            stringPutHexA(timeSt+6, ourTime->tm_sec, 2);
            stringPutHexA(timeSt+3, ourTime->tm_min, 2);
            stringPutHexA(timeSt, ourTime->tm_hour, 2);
            pal16[1] = 0xffff;
            graphicsStringA(&screenBitmap2, 4 + 41*6, 2, &sprite5_7, std5x7_, 6, 0,
                        timeSt);

            b = powerGetStatusA();
            stringPutHexA(powerSt, b, 4);

            if (powerIsDCConnectedA()) {
                powerSt[4] = '+';                
            } else {
                powerSt[4] = ' ';
            }
            pal16[1] = 0xffff;
            graphicsStringA(&screenBitmap2, 4 + 35*6, 2, &sprite5_7, std5x7_, 6, 0,
                        powerSt);
                       
            // Update file display window if needed,
            
            if (mode==0 && cursorMoved==1) {
                for (c=0;c<16;c++) {
                    if ((dirpos+c)>=totalEntries) break;

                    pal16[1] = 0xffff;
                    if (dirBuffer[dirpos+c].attr & FAT_ATTR_DIR)
                        pal16[1] = 0x4e4e;
                    if (c==cursorpos) {
                        pal16[0] = 0x0000;
                    } else {
                        pal16[0] = 0x1414;            
                    }
    
                    graphicsStringA(&screenBitmap, 3, 3 + c*8, &sprite5_7, std5x7_, 5, 0,
                        dirBuffer[dirpos+c].name);
                }
                cursorMoved=0;
            }
                
                
            c =buttonsGetStatusA();
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
					if(strcmp(nameCur,"/")!=0)	{
						char * namePos=strrchr(nameCur+1,'/');
						if ( namePos )
							*namePos = 0;
						namePos=strrchr(nameCur,'/');
						if ( namePos )
							*(namePos+1) = 0;
						cursorMoved=1;
						do {
							c =buttonsGetStatusA();
						} while(c & BUTTONS_AV300_ANY);
						break;
                    }
                } else if (c & BUTTONS_AV300_RIGHT) {
                    if (dirBuffer[dirpos+cursorpos].attr & FAT_ATTR_DIR) {
                        if(strcmp(dirBuffer[dirpos+cursorpos].name,"..")==0)
						{
							char * namePos=strrchr(nameCur+1,'/');
							if ( namePos )
							{
								*namePos = 0;
							}
							namePos=strrchr(nameCur,'/');
							if ( namePos )
							{
								*(namePos+1) = 0;
							}

						}
						else
						{
							strcat(nameCur,dirBuffer[dirpos+cursorpos].name);
							strcat(nameCur,"/");
						}
                        cursorMoved=1;
                        do {
                            c =buttonsGetStatusA();
                        } while(c & BUTTONS_AV300_ANY);
                        break;
                    }
                } else if (c & BUTTONS_AV300_ON) {
                    if (dirBuffer[dirpos+cursorpos].attr & FAT_ATTR_DIR) {
                        if(strcmp(dirBuffer[dirpos+cursorpos].name,"..")==0)
						{
							char * namePos=strrchr(nameCur+1,'/');
							if ( namePos )
							{
								*namePos = 0;
							}
							namePos=strrchr(nameCur,'/');
							if ( namePos )
							{
								*(namePos+1) = 0;
							}

						}
						else
						{
							strcat(nameCur,dirBuffer[dirpos+cursorpos].name);
							strcat(nameCur,"/");
						}
                        cursorMoved=1;
                        do {
                            c =buttonsGetStatusA();
                        } while(c & BUTTONS_AV300_ANY);
                        break;
                    } else {
                        // TODO
                        //  Handle plugins
                        //  text file reader
                        //  image reader
                        //  mp3 player
                        //  etc

                        // for now, RUN IT!
                        char fileN[MAX_PATH];
						strcpy(fileN,nameCur);
						strcat(fileN,dirBuffer[dirpos+cursorpos].name);
						int i;

						for(i=0;i<nbExt;i++)
						{
							if(strcmp(dirBuffer[dirpos+cursorpos].ext,ext[i])==0)
							{
								if(action[i](fileN))
									goto startInit;
							}
						}

                            goto startInit;


                    }
                    do {
                        c =buttonsGetStatusA();
                    } while(c & BUTTONS_AV300_ANY);
                }
            }
            
            // Other non mode 0 specific stuff...

            if (c & BUTTONS_AV300_MENU3) {
                mode ^= 1;
                do {
                    c =buttonsGetStatusA();
                } while(c & BUTTONS_AV300_ANY);
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
                
                break;
            } else if (c & BUTTONS_AV300_MENU2) {
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


                c = fatInit(getPartition(0));
                debug("[fatTest.c] fatInit returned = %x\n",c);

                nameCur[0]='/';
				nameCur[1]=0;

                do {
                    c =buttonsGetStatusA();
                } while(c & BUTTONS_AV300_ANY);
                cursorMoved=1;
                break;
            }
            for (i=0;i<loopDelay;i++) {}          // Little delay...

        }
    }
    
    debug("All done!");
    
    while(1) {}
}

int launchFile(char * fileN)
{
	debug("loading:|%s|\n",fileN);
	if(loadFile(fileN))
	{
		debug("File loaded at %x\n",0x03000000);

		char * argv[MAX_PATH];
		argv[0]=fileN;

		codeCaller(1,argv);
		return 1;
	}
	else
		return 0;

}
