// ata.h
#ifndef _ATA_H_
#define _ATA_H_

// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

extern u32 getNxtEntry( char* buffer, u32 lba);
extern u32 ataReadLBA( char* buffer, u32 lba, u32 n );
extern u32 ataWriteLBA( char* buffer, u32 lba, u32 n );
extern u32 ataRWLBA( char* buffer, u32 lba, u32 n, u32 cmd ); // cmd==1: Read, cmd==2: Write
extern void ataGetStatus();
extern u32 ataIdentify( char* buffer );
extern void ataInit();
extern void ataWaitForBSY();
extern void ataWaitForRDY();
extern void ataWaitForStartOfXfer();

// C functions

struct ataInfo{
	char serial[21];
	char firmware[9];
	char model [41];
};

struct partition {
	u32 type;
	char strType[15];
	u32 start;
	u32 size;
};

void ataIni();
int ataIdent();
int ataReadMBR();
void showAtaInfo();
void showPartitionInfo();
struct partition * getPartition(int i);

#endif
