#include <ata.h>
#include <fat.h>
#include <uart.h>

static char boot[512];
static int fatSize;
static int numFats;
static int rsvdSecCnt;
static int rootClu;
static int secPerClu;

static int LBAFat1;
static int LBAData;

static int fatCacheLBA=-1;
static int fatCache[128];                     // 1 sector fatcache

char hex8[] = "xxxxxxxx";

int getRootClu() {
    return rootClu;
}

int fatDirFilter(struct dirEntry dirIn[], struct dirEntry dirOut[], int n) {
    int cpin=0,cpo=0;

    for (cpin=0;cpin<n;cpin++) {
        if (dirIn[cpin].name[0]==0) {
            break;    
        } else if (dirIn[cpin].name[0]!=0xe5) {
            if (!(dirIn[cpin].attr & FAT_ATTR_VOLUME_ID) &&
               (dirIn[cpin].attr!= FAT_ATTR_LONG_NAME)) {
                dirOut[cpo++] = dirIn[cpin];
            }
        }
        
    }
    return cpo;
}


int fatInit(u32 lba) {
    int c;
    
    fatCacheLBA=-1;
    
    c=ataReadSectors(lba, 1, boot);      // Read the bootsector

    if (c!=ATA_ERROR_NONE) {
        uartOuts("ataReadSectors returned error!\n");
        return c;
    }

    fatSize = bootRead(36, 4);
    stringPutHex(hex8, fatSize, 8);
    uartOuts("[fat.c] FATSz32 = ");
    uartOuts(hex8);
    uartOuts("\n");

    numFats = bootRead(16, 1);
    stringPutHex(hex8, numFats, 8);
    uartOuts("[fat.c] NumFATs = ");
    uartOuts(hex8);
    uartOuts("\n");

    rsvdSecCnt = bootRead(14, 2);
    stringPutHex(hex8, rsvdSecCnt, 8);
    uartOuts("[fat.c] rsvdSecCnt = ");
    uartOuts(hex8);
    uartOuts("\n");

    LBAFat1 = lba + rsvdSecCnt;

    rootClu = bootRead(44, 4);
    stringPutHex(hex8, rootClu, 8);
    uartOuts("[fat.c] rootClu = ");
    uartOuts(hex8);
    uartOuts("\n");

    LBAData = LBAFat1 + (fatSize * numFats);
    
    secPerClu = bootRead(13, 1);
    stringPutHex(hex8, secPerClu, 8);
    uartOuts("[fat.c] secPerClu = ");
    uartOuts(hex8);
    uartOuts("\n");

    stringPutHex(hex8, LBAFat1, 8);
    uartOuts("[fat.c] LBAFat1 = ");
    uartOuts(hex8);
    uartOuts("\n");

    stringPutHex(hex8, LBAData, 8);
    uartOuts("[fat.c] LBAData = ");
    uartOuts(hex8);
    uartOuts("\n");
    
    return 0;
}

int fatReadCluster(int cluster, char* buffer) {
    int sec = LBAData + ((cluster - 2) * secPerClu);
    return ataReadSectors(sec, secPerClu, buffer);
    
    // Use safe sector by sector mode for now (SLOW....) eugh need to fix multi
    //int c,i;
    //int sec = LBAData + (cluster * secPerClu);
    //for (i=0;i<secPerClu;i++) {
    //    c = ataReadSectors(sec, 1, buffer);
    //    if (c!=ATA_ERROR_NONE) return c;
    //    sec++;
    //    buffer+=512;
    //}
    //return c;
}


int fatTrace(int cluster) {
    int lba, c;
    lba = LBAFat1 + ((cluster*4) >> 9);   // 4 - 32bit, 9-512 byte sectors.

    stringPutHex(hex8, lba, 8);
    uartOuts("[fat.c] fatTrace.lba = ");
    uartOuts(hex8);
    uartOuts("\n");
    
    if (fatCacheLBA != lba) {
        c = ataReadSectors(lba, 1, (char*) fatCache);    
        if (c!=ATA_ERROR_NONE) return c;    // Error!
        fatCacheLBA = lba;
    }
    
    return fatCache[cluster & 127];
}

int fatReadFile(int cluster, char* buffer) {
    int c;
    while(1) {    
        stringPutHex(hex8, cluster, 8);
        uartOuts("[fat.c] reading cluster ");
        uartOuts(hex8);
        uartOuts("\n");
        
        c = fatReadCluster(cluster, buffer);        // Read data...
        if (c!=ATA_ERROR_NONE) return c;
        buffer = buffer + (secPerClu*512);      // Move along,
        cluster = fatTrace(cluster);            // Trace the fat
        if (cluster<0) return cluster;          // ATA Error!
        if (cluster>0x0ffffff0) return 0;       // End of chain...
    }
}


int bootRead(int addr, int n) {
    int v=0;
    int c=0;
    for (c=0;c<n;c++) {
        v = (v << 8) | boot[addr + n - 1 - c]; 
    }
    return v;
}
