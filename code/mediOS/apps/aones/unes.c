/*************************************
  Little John 
  File : unes.c
  Authors : Yoyo.
  Version : 0.4
  Last update : 4th October 2003
**************************************/

#include "medios.h"
#include "datatypes.h"
#include "dspshared.h"

#include "aones.h"
#include "snss.h"
#include "unes.h"
#include "unes_ppu.h"
#include "unes_mapper.h"
#include "unes_io.h"
#include "unes_crc32.h"
#include "nes_apu.h"

#include "unes_db.h"

#ifndef __asmcpu__
static nes6502_memread _NESread[] =
{
   /* $0 - $7FF is RAM */
   { 0x0800, 0xFFFF, Rd6502 },
   { -1,     -1,     NULL }
};
static nes6502_memwrite _NESwrite[] =
{
   /* $0 - $7FF is RAM */
   { 0x0800, 0xFFFF, Wr6502 },
   { -1,     -1,     NULL }
};
static nes6502_context _NEScontext;

#endif

// Mapper
//extern Mapper_Info mapfunc;
mmc_t *mapper;

// Emu stuff

long romsize;

__IRAM_DATA bool framecompleted;
__IRAM_DATA uint32  curscanline;
__IRAM_DATA uint32  synccycles;

char lj_curVolume;
nestoy_info ninfo;

typedef struct {int crc_all;int crc;char name[255];unsigned char header1;unsigned char header2;
unsigned char rom_bank_nb;unsigned char chr_bank_nb;
char country[6];char editor[64];} t_nestoy_entry;
t_nestoy_entry tinfo;
uint8 nestoy_getinfo(uint32 crc,uint32 crcall,t_nestoy_entry *ninfo);


uint32 CYCLES_BEFORE_NMI=1;

char chaine[1024];
char chaineDEB[1024];

inline uint8 emulate_one_framePAL(void);
inline uint8 emulate_one_frameNTSC(void);

void nes_reset(void);
void CloseAll(void);
int SaveStateSnss(byte b);
int LoadStateSnss(byte b);
void LoadSaveSRAM(bool save);

void reset_asmcpu(void);

#include "unes_ROM_Correct.h"

char Init_NES(char *RomName)
{
    printf("Init NES\n");
  
    memset(Vnes.PPU_patterntables,0,0x8000);
    memset(Vnes.PPU_nametables,0,0x1000);
    memset(Vnes.PPU_palette,0,0x100);
  
    memset(Vnes.CPUMemory,0,65536);  
  
    memset(Vnes.NESSRAM,0,0x10000);
    memset(Vnes.mapper_extram,0,0x10000);

    memset(Vnes.var.Vbuffer,0,NES_BUFFER_WIDTH*NES_PAL_HEIGHT);
    memset(Vnes.var.Vbuffer2,0,NES_BUFFER_WIDTH*NES_PAL_HEIGHT);
    memset(Vnes.var.LineOffset,0,NES_PAL_HEIGHT*sizeof(uint32));

    Vnes.sramsize=0x2000;
    Vnes.vramsize=0x2000;

  
    //and the mirror, according to the method choosen, 4screen not implemented yet!!!!
    if (Vnes.ROM_Header.rom_ctrl1&1)
    {
        //Vertical Vnes.var.mirroring : name table 0,1 are orginals, 2,3 are mirrors
        Vnes.var.mirroring=1;
    }
    else
    {
        //Horizontal Vnes.var.mirroring : name table 0,2 are orginals, 1,3 are mirrors
        Vnes.var.mirroring=2;
    }
    if (Vnes.ROM_Header.rom_ctrl1&8) Vnes.var.mirroring=5;

    //set vram
    Vnes.var.vrom=0;

    //set low cpu page
    Vnes.CPUPageIndex[0]=Vnes.CPUMemory;
    Vnes.CPUPageIndex[1]=Vnes.CPUMemory+8192;
    Vnes.CPUPageIndex[2]=Vnes.CPUMemory+16384;
    Vnes.CPUPageIndex[3]=Vnes.NESSRAM; //Vnes.CPUMemory+16384+8192;
  
    if (Vnes.trainer) memcpy((uint8*)(&Vnes.CPUPageIndex[3][0x1000]),(uint8*)(Vnes.rom+16),512);
  
  
    mapper=(mmc_t *)mmc_create();
    if (!mapper)
    {
      sprintf(chaine,"Mapper %d not supported\n",Vnes.Mapper_used);
      DEBUGS(chaine);

      return 1;
    }

    printf("Using mapper #%d(%s)\n",mapper->intf->number,mapper->intf->name);

    ppu_init();
  
#ifndef __asmcpu__
    nes6502_getcontext(&_NEScontext);
    _NEScontext.mem_page[0]=(uint8 *)Vnes.CPUPageIndex[0];
    /*_NEScontext.mem_page[1]=(uint8 *)Vnes.CPUPageIndex[1];
    _NEScontext.mem_page[2]=(uint8 *)Vnes.CPUPageIndex[2];
    _NEScontext.mem_page[3]=(uint8 *)Vnes.CPUPageIndex[3];
    _NEScontext.mem_page[4]=(uint8 *)Vnes.CPUPageIndex[4];
    _NEScontext.mem_page[5]=(uint8 *)Vnes.CPUPageIndex[5];
    _NEScontext.mem_page[6]=(uint8 *)Vnes.CPUPageIndex[6];
    _NEScontext.mem_page[7]=(uint8 *)Vnes.CPUPageIndex[7];*/
    _NEScontext.read_handler=_NESread;
    _NEScontext.write_handler=_NESwrite;
    nes6502_setcontext(&_NEScontext);
#endif
      
    Vnes.var.DrawCframe=1;
  
    Vnes.var.JoyPad1_BitIndex=0;
    Vnes.var.JoyPad2_BitIndex=0;    
      
    Vnes.var.SaveRAM=Vnes.ROM_Header.rom_ctrl1&2;

    return 0;
}

char Load_ROM(char *RomName)
{
    int f;
  
    printf("Loading ROM %s\n",RomName);
  
    f=open(RomName,O_RDONLY);
    if (f<0) {
        DEBUGS("error at opening\n");
        return 1;
    }
  
    romsize=filesize(f);
  
    Vnes.rom=malloc(romsize);
  
    if (!Vnes.rom)
    {
        DEBUGS("error\n");
        close(f);
        return(2);
    }
  
    /* Read file data */
    uint8 *ptmp;
    ptmp=(uint8*)malloc(32768);
    int32 l=romsize;
    int32 FileSize=0;
  
    while (l)
    {
        read (f,ptmp, 32768);
        if (l>=32768)
        {
            memcpy(Vnes.rom+FileSize,ptmp,32768);
  
            FileSize+=32768;
            l-=32768;
        }
        else if (l)
        {
            memcpy(Vnes.rom+FileSize,ptmp,l);
            l=0;
        }
    }
    free(ptmp);
  
  
    close(f);
  
    return 0;
}

char Open_ROM()
{
    int prevcpucycle;

    Vnes.romsize=romsize;

    printf("Opening ROM, size=%dKB\n",romsize/1024);

    if ((Vnes.rom[0]!='N')||(Vnes.rom[1]!='E')||(Vnes.rom[2]!='S'))
    {
        return(4);
    }

    //test for a .nsf file
    if (Vnes.rom[3]=='M')
    {
        Vnes.var.nsfmode=1;
    }
    else
    {
        Vnes.var.nsfmode=0;
    }

    memset(&Vnes.ROM_Header,0,16);

    Vnes.var.crc = 0;
    Vnes.var.crc_all = 0;
/*gli
    Vnes.var.country[0]='?';
    Vnes.var.country[1]=0;
    Vnes.var.publisher[0]='?';
    Vnes.var.publisher[1]=0;
    Vnes.var.date[0]='?';
    Vnes.var.date[1]=0;
*/

    if (!Vnes.var.nsfmode)
    {
        memcpy(&Vnes.ROM_Header,Vnes.rom,16);
        Vnes.trainer = (Vnes.ROM_Header.rom_ctrl1&0x4?1:0);

        Vnes.var.chr_beg=Vnes.rom+16+Vnes.trainer*512+Vnes.ROM_Header.prg_rom_pages_nb*16384;
        Vnes.var.prg_beg=Vnes.rom+16+Vnes.trainer*512;
        Vnes.var.cart_size=(romsize-16-Vnes.trainer*512)/1024;

        Vnes.var.crc = CrcCalc(Vnes.rom+16, Vnes.trainer*512+Vnes.ROM_Header.prg_rom_pages_nb * 0x4000);//, Vnes.var.crc);
        Vnes.var.crc_all = CrcCalca(Vnes.var.chr_beg, Vnes.ROM_Header.chr_rom_pages_nb * 0x2000, Vnes.var.crc);

        /* gli
        if (!nestoy_getinfo(Vnes.var.crc,Vnes.var.crc_all,&tinfo))
        {
            strcpy(gamename,tinfo.name);
            strcpy(Vnes.var.country,tinfo.country);
            strcpy(Vnes.var.publisher,tinfo.editor);
            //strcpy(Vnes.var.date,ninfo.date);
            if ((Vnes.var.country[0]=='E')||(Vnes.var.country[0]=='S'))
            {
                Vnes.var.fps=50;
                Vnes.var.frame_time=1000/Vnes.var.fps;
            }
            Vnes.ROM_Header.rom_ctrl1=tinfo.header1;
            Vnes.ROM_Header.rom_ctrl2=tinfo.header2;
        }
        */

        prevcpucycle=Vnes.var.cpucycle;
        Vnes.var.cpucycle=0;

        Crc_CorrectRomSettings();

        //gli: ignore gui setting when there's a game fix
        if (Vnes.var.cpucycle!=0){
            Vnes.var.CustomCpuCycle=true;
        }else{
            Vnes.var.cpucycle=prevcpucycle;
            Vnes.var.CustomCpuCycle=false;
        }


        if (Vnes.rom[12]==0)
            Vnes.Mapper_used=(Vnes.ROM_Header.rom_ctrl1>>4)+(Vnes.ROM_Header.rom_ctrl2&0xf0);
        else
            Vnes.Mapper_used=(Vnes.ROM_Header.rom_ctrl1>>4);
    }
    else
    {
        //pass nsf size to nsf mapper
        long nsfsize=romsize-0x80;
        Vnes.rom[0]=nsfsize&0xff;
        Vnes.rom[1]=(nsfsize>>8)&0xff;
        Vnes.rom[2]=(nsfsize>>16)&0xff;

        Vnes.var.prg_beg=Vnes.rom;
        Vnes.Mapper_used=256; // NSF player dummy mapper
    };


    if ( (!Vnes.CPUMemory)||(!Vnes.PPU_patterntables)||(!Vnes.PPU_nametables)||(!Vnes.PPU_palette)||
      (!Vnes.mapper_extram)||(!Vnes.NESSRAM))
    {
        return(5);
    }

    return(0);
}

void LoadSaveSRAM(bool save)
{
    int f;
    int i,j,k;
    char * sramfile = malloc(256);
    char * result = malloc(256);

    i=0;
    j=255;
    k=-1;
    while (CurrentROMFile[i]!=0)
    {
        if (CurrentROMFile[i]=='.') j=i;
        if (CurrentROMFile[i]=='/') k=i;
        if (CurrentROMFile[i]=='\\') k=i;
        i++;
    }
    if (j==255) j=i;  //pas d'extension
    memcpy(sramfile,CurrentROMFile+k+1,j+1-k-1);
    sramfile[j+1-k-1]=0;
    strcat(sramfile,"srm");

    sprintf(result,SAVES_PATH"%s",sramfile);

    if(save){
        printf("Saving SRAM...\n");

        f = open(result,O_RDWR|O_CREAT);
        if (f>=0)
        {
            write(f,Vnes.NESSRAM ,0x2000);
            close(f);
        }
    }else{
        printf("Loading SRAM...\n");

        f = open(result,O_RDONLY);
        if (f>=0)
        {
            read(f,Vnes.NESSRAM ,0x2000);
            close(f);
        }
    }

    free(result);
    free(sramfile);
}

char Close_ROM(int b)
{
    printf("Closing ROM\n");

    mmc_destroy(&mapper);

    if (Vnes.rom) free(Vnes.rom);

    return(0);
}

void Reset_NES()
{
    printf("Reset NES\n");

    Vnes.var.sramEnabled=Vnes.var.SaveRAM;

    ppu_reset(true);

    printf("ppu\n");

    apu_reset();

    printf("apu\n");

    mmc_reset();

    printf("mmc\n");

    if (ppu_noLatchFunc()) Vnes.var.lazy_mode_avail=1;
    else Vnes.var.lazy_mode_avail=0;

    if (Vnes.var.lazy_mode_avail) Vnes.var.lazy_mode=1;
    else Vnes.var.lazy_mode=0;

    Vnes.var.frame_irq_enabled = 0xFF;
    Vnes.var.frame_irq_disenabled = 0;


#ifdef __asmcpu__
    reset_asmcpu();
#else
    nes6502_reset();
#endif

    printf("cpu\n");

    Vnes.var.curdeccycle=0;
    Vnes.var.currentcpucycle=0;
}

void Run_NES()
{
    bool wantQuit;

    wantQuit=false;
    do{
        if (Vnes.var.fps==60)
            wantQuit=emulate_one_frameNTSC();
        else
            wantQuit=emulate_one_framePAL();
    }while(!wantQuit);
}

void GetStateFileName(int num,char *str)
{
    char * result=malloc(256);
    int i;
    int beg,end;

    i=0;
    beg=0;
    end=-1;

    while (CurrentROMFile[i])
    {
        if (CurrentROMFile[i]=='/') beg=i;
        if (CurrentROMFile[i]=='.') end=i;
        i++;
    }
    if (end==-1) end=i;  //pas d'extension

    memcpy(result,CurrentROMFile+beg+1,end+1-beg-1);
    result[end+1-beg-1]=0;
    sprintf(str,SAVES_PATH"%sss%d",result,num);

    free(result);
}

int SaveStateSnss(byte b)
{
    SNSS_FILE *f;
    char * savename=malloc(256);
    int i;
#ifdef __asmcpu__
    uint32 nesregs[6];
#endif

    SNSS_RETURN_CODE code;

    GetStateFileName(b,savename);

    code = SNSS_OpenFile (&f,savename,SNSS_OPEN_WRITE);
    if (code != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        return 1;
    }

    printf("Saving state %s\n",savename);

    printf("writeheader\n");

#ifndef __asmcpu__
    nes6502_getcontext(&_NEScontext);
    f->baseBlock.regA=_NEScontext.a_reg;
    f->baseBlock.regX=_NEScontext.x_reg;
    f->baseBlock.regY=_NEScontext.y_reg;
    f->baseBlock.regFlags=_NEScontext.p_reg;
    f->baseBlock.regStack=_NEScontext.s_reg;
    f->baseBlock.regPc=_NEScontext.pc_reg;
#else
    get_context(nesregs);
    f->baseBlock.regA=nesregs[0];
    f->baseBlock.regX=nesregs[1];
    f->baseBlock.regY=nesregs[2];
    f->baseBlock.regFlags=nesregs[3];
    f->baseBlock.regStack=nesregs[5];
    f->baseBlock.regPc=nesregs[4];
#endif
  
    printf("got context\n");
  
    memcpy(f->baseBlock.cpuRam,Vnes.CPUPageIndex[0],0x800);
    ppu_savestate(&(f->baseBlock));
  
    printf("got ppu\n");
  
    memcpy(f->baseBlock.ppuRam,Vnes.PPU_nametables,0x1000);
    memcpy(f->baseBlock.palette,Vnes.PPU_palette,0x20);
    for (i=0;i<0x20;i++)
    {
        f->baseBlock.palette[i]&=0x3f;
    }
    f->baseBlock.mirrorState[0]=Vnes.var.ppu_mirror0;
    f->baseBlock.mirrorState[1]=Vnes.var.ppu_mirror1;
    f->baseBlock.mirrorState[2]=Vnes.var.ppu_mirror2;
    f->baseBlock.mirrorState[3]=Vnes.var.ppu_mirror3;

    printf("write base block\n");

    if ((code = SNSS_WriteBlock(f,SNSS_BASR)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }


    f->vramBlock.vramSize=Vnes.vramsize;
    memcpy(f->vramBlock.vram,Vnes.PPU_patterntables,f->vramBlock.vramSize);

    printf("write vram block\n");

    if ((code = SNSS_WriteBlock(f,SNSS_VRAM)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }
  
  
    f->sramBlock.sramSize=Vnes.sramsize;
    f->sramBlock.sramEnabled=Vnes.var.sramEnabled;
    memcpy(f->sramBlock.sram,Vnes.NESSRAM ,f->sramBlock.sramSize);
  
    printf("write sram block\n");
  
    if ((code = SNSS_WriteBlock(f,SNSS_SRAM)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }

    for (i = 0; i < 4; i ++)
    {
        f->mapperBlock.prgPages[i]=(Vnes.CPUPageIndex[i+4]-Vnes.var.prg_beg) >> 13;
    }

    for (i = 0; i < 8; i ++)
    {
        if (Vnes.PPUPageIndexProtect[i])
          f->mapperBlock.chrPages[i]=(Vnes.PPUPageIndex[i]-Vnes.var.chr_beg) >> 10;
        else
          f->mapperBlock.chrPages[i]=((Vnes.PPUPageIndex[i]-Vnes.PPU_patterntables) >> 10) | 0x8000;
    }

    if (mapper->intf->get_state) mapper->intf->get_state(&(f->mapperBlock));

    printf("write mapper block\n");

    if ((code = SNSS_WriteBlock(f,SNSS_MPRD)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }
  
    memcpy(f->soundBlock.soundRegisters,&Vnes.CPUMemory[0x4000],0x16);
  
    printf("write sound block\n");
  
    if ((code = SNSS_WriteBlock(f,SNSS_SOUN)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }
    
    printf("closing file\n");
    /* close the files */
    if ((code=SNSS_CloseFile(&f)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }

    free(savename);
    return 0;
}

int LoadStateSnss(byte b)
{
    SNSS_FILE *f;
    char *savename=malloc(256);
    int i,j;
#ifdef __asmcpu__
    uint32 nesregs[6];
#endif
  
    SNSS_RETURN_CODE code;
    SNSS_BLOCK_TYPE blockType;
  
    GetStateFileName(b,savename);
  
    code = SNSS_OpenFile (&f,savename,SNSS_OPEN_READ);
  
    if (code!=SNSS_OK)
    {
        free(savename);
        return 1;
    }

    printf("Loading state %s\n",savename);

    /* iterate through each block in the file */
    if (f->headerBlock.numberOfBlocks>255)
    {
        //printf("Load overflow\n");
        free(savename);
        return 1;
    }

     // now we are too far to not reset the nes if there is a pb
    Reset_NES();

    for (j = 0; j < f->headerBlock.numberOfBlocks; j++)
    {

        if ((code = SNSS_GetNextBlockType(&blockType,f)) != SNSS_OK)
        {
            printf ("Error: %s\n", SNSS_GetErrorString (code));
            free(savename);
            return 1;
        }

        if ((code = SNSS_ReadBlock(f,blockType)) != SNSS_OK)
        {
            printf ("Error: %s\n", SNSS_GetErrorString (code));
            free(savename);
            return 1;
        }

        switch (blockType)
        {
            case SNSS_BASR:

           // sprintf(chaine,"reading base registers block\n");
#ifndef __asmcpu__
                nes6502_getcontext(&_NEScontext);
                _NEScontext.a_reg=f->baseBlock.regA;
                _NEScontext.x_reg=f->baseBlock.regX;
                _NEScontext.y_reg=f->baseBlock.regY;
                _NEScontext.p_reg=f->baseBlock.regFlags;
                _NEScontext.s_reg=f->baseBlock.regStack;
                _NEScontext.pc_reg=f->baseBlock.regPc;
                _NEScontext.int_pending = 0;
                nes6502_setcontext(&_NEScontext);
#else
                nesregs[0]=f->baseBlock.regA;
                nesregs[1]=f->baseBlock.regX;
                nesregs[2]=f->baseBlock.regY;
                nesregs[3]=f->baseBlock.regFlags;
                nesregs[4]=f->baseBlock.regPc;
                nesregs[5]=f->baseBlock.regStack;
                set_context(nesregs);
#endif
                memcpy(Vnes.CPUPageIndex[0],f->baseBlock.cpuRam,0x800);
                ppu_loadstate(&(f->baseBlock));
                memcpy(Vnes.PPU_nametables,f->baseBlock.ppuRam,0x1000);
                memcpy(Vnes.PPU_palette,f->baseBlock.palette,0x20);

                for (i=0;i<0x10;i++)
                {
                    Vnes.PPU_palette[i]&=0x3f;
                    Vnes.PPU_palette[i+0x10]&=0x3f;
                }

                Vnes.var.ppu_mirror0=f->baseBlock.mirrorState[0]&0x03;
                Vnes.var.ppu_mirror1=f->baseBlock.mirrorState[1]&0x03;
                Vnes.var.ppu_mirror2=f->baseBlock.mirrorState[2]&0x03;
                Vnes.var.ppu_mirror3=f->baseBlock.mirrorState[3]&0x03;
                ppu_mirror(Vnes.var.ppu_mirror0,Vnes.var.ppu_mirror1,Vnes.var.ppu_mirror2,Vnes.var.ppu_mirror3);
                break;

          case SNSS_VRAM:
              //sprintf(chaine,"reading VRAM block, %d\n",f->vramBlock.vramSize);
              //gpprintf (0,64,chaine,-1,-1,13);
              memcpy(Vnes.PPU_patterntables,(f->vramBlock.vram),f->vramBlock.vramSize);
              break;

          case SNSS_SRAM:
              //gpprintf (0,16*5,"reading SRAM block\n",-1,-1,13);
              sprintf(chaine,"reading SRAM block, %d\n",f->sramBlock.sramSize);
              //gpprintf (0,16*5,chaine,-1,-1,13);          
              memcpy(Vnes.NESSRAM/*Vnes.CPUMemory+0x6000*/,(f->sramBlock.sram),f->sramBlock.sramSize);
              Vnes.var.sramEnabled=f->sramBlock.sramEnabled;
              break;

          case SNSS_MPRD:
              //gpprintf (0,16*6,"reading mapper data block\n",-1,-1,13);                 

              for (i = 0; i < 4; i ++)
              {
                  Vnes.CPUPageIndex[i+4]=Vnes.var.prg_beg+((uint32)f->mapperBlock.prgPages[i]<<13);
              }          
              // set CHR pages
              for(i = 0; i < 8; i++)
              {
                  if(f->mapperBlock.chrPages[i] & 0x8000)
                  {
                      // VRAM
                      Vnes.PPUPageIndex[i] = Vnes.PPU_patterntables + ((uint32)(f->mapperBlock.chrPages[i] & 0x7fff) << 10);
                      Vnes.PPUPageIndexProtect[i]=0;
                  }
                  else
                  {
                      // VROM
                      Vnes.PPUPageIndex[i] = Vnes.var.chr_beg + ((uint32)(f->mapperBlock.chrPages[i]) << 10);
                      Vnes.PPUPageIndexProtect[i]=1;
                  }
              }
              if (mapper->intf->set_state) mapper->intf->set_state(&(f->mapperBlock));

#ifdef __asmcpu__
              set_cpu_bank_full(Vnes.CPUPageIndex[4],Vnes.CPUPageIndex[5],Vnes.CPUPageIndex[6],Vnes.CPUPageIndex[7]);
#else
              nes6502_getcontext(&_NEScontext);
              nes6502_setcontext(&_NEScontext);
#endif
              break;

          case SNSS_CNTR:
              //gpprintf (0,16*7,"reading controllers block\n",-1,-1,13);
              break;

          case SNSS_SOUN:
              //gpprintf (0,16*8,"reading sound block\n",-1,-1,13);
              //ResetSoundState();
              for (i = 0x15; i >=0; i --) //gli: reverse order to get SMASK written first (fixes lots of problems)
              {
                  if(i == 0x14) continue;
                  Vnes.CPUMemory[0x4000+i]=f->soundBlock.soundRegisters[i];
#if 0
                  // write the DMC regs directly
                  if((i >= 0x10) && (i <= 0x13))
                  {
                      apu->apus.dmc.regs[i - 0x10] = f->soundBlock.soundRegisters[i];
                  }
                  else
                  {
                      apu_write(0x4000 + i, f->soundBlock.soundRegisters[i]);
                      apu_write_cur(0x4000 + i, f->soundBlock.soundRegisters[i]);
                  }
#else
                  apu_write(0x4000 + i, f->soundBlock.soundRegisters[i]);
#endif
              }
              break;

          default:
              //printf ("invalid block type\n");
              break;
          }
      }

    /* close the files */
    if ((code = SNSS_CloseFile (&f)) != SNSS_OK)
    {
        printf ("Error: %s\n", SNSS_GetErrorString (code));
        free(savename);
        return 1;
    }

    free(savename);
    return 0;
}

__IRAM_CODE inline uint8 emulate_one_frameNTSC(void)
{
    framecompleted=false;

    while (!framecompleted)
    {
        curscanline=ppu_currentscanline();

        Vnes.var.currentcpucycle+=Vnes.var.cpucycle;
        Vnes.var.curdeccycle+=Vnes.var.deccycle;
        if (Vnes.var.curdeccycle>=65536)
        {
            Vnes.var.currentcpucycle++;
            Vnes.var.curdeccycle-=65536;
        }
        if (Vnes.var.currentcpucycle>0)
        {
            synccycles=Vnes.var.currentcpucycle;
            if (curscanline==241) synccycles+=CYCLES_BEFORE_NMI;
            #ifndef __asmcpu__
            Vnes.var.currentcpucycle-=nes6502_execute(Vnes.var.currentcpucycle);
            #else
            Vnes.var.currentcpucycle-=cpu_exec(Vnes.var.currentcpucycle);
            #endif
            synccycles=0;
        }

#ifdef SOUND_USE_DSP
        // for snd sync on dsp
        dsp_write32(&dspCom->cpuCurCycle,CPU_GET_CYCLES());
#endif

        if (mapper->intf->HSync) mapper->intf->HSync(curscanline);

        ppu_lazy_scanlineNTSC();

        switch (curscanline)
        {
            case 240:
                //framecompleted=true;
                if(!(Vnes.var.frame_irq_enabled & 0xC0))
                {
                    #ifndef __asmcpu__
                    nes6502_pending_irq();
                    #else
                    cpu_pending_irq();
                    #endif
                }

                if(Vnes.var.DrawCframe){
                    emu_handleVideoBuffer();
                }

                break;
            case 241:
                //cpu vbl
                if (mapper->intf->VSync) mapper->intf->VSync();

                #ifndef __asmcpu__
                Vnes.var.currentcpucycle-=nes6502_execute(CYCLES_BEFORE_NMI); //7);
                if (ppu_vbl()) nes6502_nmi();
                #else
                Vnes.var.currentcpucycle-=cpu_exec(CYCLES_BEFORE_NMI); //7);
                if (ppu_vbl()) cpu_nmi();
                #endif
                break;
            case 261:
                framecompleted=true;
                break;
        }
    }

    return emu_frameCompleted();
}


inline uint8 emulate_one_framePAL(void)
{      
  framecompleted=false;
  //vblank=true;  
  while (!framecompleted)
  {
     curscanline=ppu_currentscanline();

     Vnes.var.currentcpucycle+=Vnes.var.cpucycle;
     Vnes.var.curdeccycle+=Vnes.var.deccycle;
     if (Vnes.var.curdeccycle>=65536)
     {
    Vnes.var.currentcpucycle++;
    Vnes.var.curdeccycle-=65536;
     }
     if (Vnes.var.currentcpucycle>0) 
     {      
    synccycles=Vnes.var.currentcpucycle;
    if (curscanline==241) synccycles+=CYCLES_BEFORE_NMI;
    #ifndef __asmcpu__
    Vnes.var.currentcpucycle-=nes6502_execute(Vnes.var.currentcpucycle);
    #else
    Vnes.var.currentcpucycle-=cpu_exec(Vnes.var.currentcpucycle);
        #endif
        synccycles=0;
     }

     if (mapper->intf->HSync) mapper->intf->HSync(curscanline);                  
     if (Vnes.var.lazy_mode) ppu_lazy_scanlinePAL();
     else ppu_scanlinePAL();
     switch (curscanline)
     {
    case 240:
        //framecompleted=true;
        if(!(Vnes.var.frame_irq_enabled & 0xC0))
        {
            #ifndef __asmcpu__          
            nes6502_pending_irq();
            #else       
            cpu_pending_irq();
            #endif
        }       
        break;
    case 241:
        //cpu vbl
        if (mapper->intf->VSync) mapper->intf->VSync();
                            
        #ifndef __asmcpu__
        Vnes.var.currentcpucycle-=nes6502_execute(CYCLES_BEFORE_NMI); //7);     
        if (ppu_vbl()) nes6502_nmi();       
        #else       
        Vnes.var.currentcpucycle-=cpu_exec(CYCLES_BEFORE_NMI); //7);
        if (ppu_vbl()) cpu_nmi();
        #endif                                      
        break;
    case 311:
        //vblank=false;
        framecompleted=true;
        break;
    /*default:
        break;*/
    }

    
    
  }
                      
  return emu_frameCompleted();
}


void CloseAll()
{
    //if (!FrmAlert(FrmAlertSaveState)) SaveStateSnss(0);

    /**/
    ppu_deinit();

    /**/
    /*close rom*/
    Close_ROM(1);
}

void panic(void)
{
}

void asm_panic(uint32 opcode,uint32 PC,uint8 *nesstack,uint8 *pcmem)
{
    sprintf(chaine,"PANIC : op %02X PC=%02X\nStack mem : %02X %02X %02X %02X\nPC mem : %02X %02X %02X %02X %02X %02X\n%02X %02X %02X %02X %02X %02X",
    opcode,PC,nesstack[0],nesstack[1],nesstack[2],nesstack[3],
    pcmem[-6],pcmem[-5],pcmem[-4],pcmem[-3],pcmem[-2],pcmem[-1],
    pcmem[0],pcmem[1],pcmem[2],pcmem[3],pcmem[4],pcmem[5]);


//    exit(1);
    for(;;);
}


//extern long __CodeRelocStart__[];

void reset_asmcpu(void)
{           
#ifdef __asmcpu__
    Vnes.NESRAM=NULL;

    init_cpu((uint8*)(&Vnes.NESRAM));
    Vnes.CPUPageIndex[0]=Vnes.NESRAM;

    init_sram(Vnes.CPUPageIndex[3]);
    //gm_memset(Vnes.NESSRAM,0,0x10000);
    memset(Vnes.NESRAM,0,0x7ff);

    reset_cpu();
#endif
}


void Cdebug_opcode(uint32 *nesreg)
{
#ifdef __asmcpu__
    sprintf(chaine,"A=%x X=%x Y=%x S%x P=%x PC=%x r0=%x ADDR=%x\n",
        nesreg[0]>>24,nesreg[1],nesreg[2],nesreg[4]>>24,nesreg[5],nesreg[3],nesreg[11],nesreg[7]);
#endif

    DEBUGS(chaine);
//  gpprintf(16,16,chaineDEB,0,0,0xe0);
}
