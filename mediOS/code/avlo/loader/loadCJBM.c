#include <file.h>
#include <debug.h>

#define HEAD_L     16
#define PASS_L     32
#define READ_L     8192

char * xorstring;
char tmp[READ_L];
int tmpOff,nbRead;
int length;

extern void drawProgress(int offset,int length,int mode);

char getNextData(int fd,int offset)
{
    drawProgress(offset,length,1);
    
    char val;
    if(tmpOff>=nbRead)
    {
        if((nbRead=fread(fd,&tmp,READ_L))<0)
        {                  
            return 1;
        }
        //debug("read %d  %d\n",nbRead,offset);
        tmpOff=0;
    }
    val=tmp[tmpOff++] ^ xorstring[offset & (PASS_L-1)];
    return val;
}

int loadCJBM(char * filename,char * key)
{
    char * dest=(char*) 0x03000000;
    unsigned char header[HEAD_L];
    int fd;
    int unpackedsize,offset,offDest;
    int bit,head;
    int x,byte1,byte2,count,src;
    
    xorstring=key;
    
    if((fd=fopen(filename,"r"))<0)
    {
        debug("Error loading file %s\n",filename);
        return 0;
    }
    
    debug("File %s opened\n",filename);
    debug("Using key:%s\n",xorstring);
    
    fread(fd,&header,HEAD_L);
    
    unpackedsize = header[4] | header[5] << 8;
    unpackedsize |= header[6] << 16 | header[7] << 24;
    
    length = header[8] | header[9] << 8;
    length |= header[10] << 16 | header[11] << 24;

    debug("length: %d, unpackedsize:%d\n",length,unpackedsize);
    
    offset=0;
    offDest=0;
    if((nbRead=fread(fd,&tmp,READ_L))<0)
    {
        debug("Can't read file\n");
        return 0;
    }
    tmpOff=0;
    debug("clust size=%d\n",getClusterSize());
    while(offset<length)
    {              
        head=getNextData(fd,offset++);
        
        for (bit=0; bit<8 && offset<length; bit++)
        {
            
            if (head & (1 << (bit))) {
                dest[offDest++] = getNextData(fd,offset++);
            }
            else {
                byte1 = getNextData(fd,offset++);
                byte2 = getNextData(fd,offset++);
                count = (byte2 & 0x0f) + 3;
                src   = (offDest & 0xfffff000) + (byte1 | ((byte2 & 0xf0)<<4)) + 18;
                if (src > offDest)
                    src -= 0x1000;

                for (x=0; x<count; x++)
                    dest[offDest++] = dest[src+x];
            }
        }
    }
     debug("Unpack end (l=%d,off=%d,offD=%d)\n",length,offset,offDest);
    return 1;  
 }
