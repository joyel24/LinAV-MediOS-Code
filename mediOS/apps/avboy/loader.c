/*
*   apps/avboy/loader.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
* 
*  Date:     18/10/2005
* Author:   GliGli

*  Modified by CjNr11 06/12/2005
*/

#include "defs.h"
#include "avboy.h"
#include "inflate.h"
#include "regs.h"
#include "mem.h"
#include "hw.h"
#include "rtc.h"
#include "lcd.h"
#include "sound.h"
#include <sys_def/string.h>
#include <sys_def/ctype.h>
#include <sys_def/random.h>





char *strdup();

static int mbc_table[256] =
{
	0, 1, 1, 1, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 3, 3, 3, 0, 0, 0, 0, 0, 5, 5, 5, MBC_RUMBLE, MBC_RUMBLE, MBC_RUMBLE, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MBC_HUC3, MBC_HUC1
};

static int rtc_table[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static int batt_table[256] =
{
	0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0,
	1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
	0
};

static int romsize_table[256] =
{
	2, 4, 8, 16, 32, 64, 128, 256, 512,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 128, 128, 128
	/* 0, 0, 72, 80, 96  -- actual values but bad to use these! */
};

static int ramsize_table[256] =
{
	1, 1, 1, 4, 16,
	4 /* FIXME - what value should this be?! */
};


static char *romfile;
static char *sramfile;
static char *rtcfile;
static char *saveprefix;

static char *savename;
static char *savedir = "/aoboy/states";

static int saveslot;

static int forcebatt, nobatt;
static int forcedmg, gbamode;

static int memfill = -1, memrand = -1;


static void initmem(void *mem, int size)
{
	char *p = mem;
	if (memrand >= 0)
	{
		srand(memrand ? memrand : -6); //time(0)
		while(size--) *(p++) = rand();
	}
	else if (memfill >= 0)
		memset(p, memfill, size);
}

static byte *loadfile(int f, int *len)
{
	int c, l = 0, p = 0;
	byte *d = 0;
//	static byte buf[64*1024];
        char * buf;

        buf=(char *)malloc(64*1024);
	for(;;)
	{
		c = read(f, buf, 64*1024); //sizeof buf);
		if (c <= 0) break;
		l += c;
		d = realloc(d, l);
		if (!d) return 0;
		memcpy(d+p, buf, c);
		p += c;
	}
	*len = l;
	free(buf);
	return d;
}

static byte *inf_buf;
static int inf_pos, inf_len;

static void inflate_callback(byte b)
{
	if (inf_pos >= inf_len)
	{
		inf_len += 512;
		inf_buf = realloc(inf_buf, inf_len);
		if (!inf_buf) DIE("out of memory inflating file @ %d bytes\n", inf_pos);
	}
	inf_buf[inf_pos++] = b;
}

static byte *decompress(byte *data, int *len)
{
	unsigned long pos = 0;
	if (data[0] != 0x1f || data[1] != 0x8b)
		return data;
	inf_buf = 0;
	inf_pos = inf_len = 0;
	if (unzip(data, &pos, inflate_callback) < 0)
		return data;
	*len = inf_pos;
	return inf_buf;
}


int rom_load()
{
	int f;
	byte c, *data, *header;
	int len = 0, rlen;


	f = open(romfile, O_RDONLY);
	if (f<0) {
       //  debug("Retry!");
       //  f = open(romfile, O_RDONLY);
       //  if (f<0) DIE("cannot open rom file: %s\n", romfile);
         DIE("cannot open rom file: %s\n", romfile);
      }
      else printf("File opened!\n");

	data = loadfile(f, &len);

      printf("Data loaded!\n");

	header = data = decompress(data, &len);
      printf("Data decompressed!\n");

	memcpy(rom.name, header+0x0134, 16);


	if (rom.name[14] & 0x80) rom.name[14] = 0;
	if (rom.name[15] & 0x80) rom.name[15] = 0;
	rom.name[16] = 0;

	c = header[0x0147];
	mbc.type = mbc_table[c];
	mbc.batt = (batt_table[c] && !nobatt) || forcebatt;
	rtc.batt = rtc_table[c];
	mbc.romsize = romsize_table[header[0x0148]];
	mbc.ramsize = ramsize_table[header[0x0149]];

	if (!mbc.romsize) DIE("unknown ROM size %02X\n", header[0x0148]);
	if (!mbc.ramsize) DIE("unknown SRAM size %02X\n", header[0x0149]);

	rlen = 16384 * mbc.romsize;
	rom.bank = realloc(data, rlen);
	if (rlen > len) memset(rom.bank[0]+len, 0xff, rlen - len);

	ram.sbank = malloc(8192 * mbc.ramsize);

	initmem(ram.sbank, 8192 * mbc.ramsize);
	initmem(ram.ibank, 4096 * 8);

	mbc.rombank = 1;
	mbc.rambank = 0;

	c = header[0x0143];
	hw.cgb = ((c == 0x80) || (c == 0xc0)) && !forcedmg;
	hw.gba = (hw.cgb && gbamode);

	if (strcmp(romfile, "-")) close(f);
      printf("Rom_load ok!\n");

	return 0;
}

int sram_load()
{
	int f;

	if (!mbc.batt || !sramfile || !*sramfile) return -1;

	/* Consider sram loaded at this point, even if file doesn't exist */
	ram.loaded = 1;

	f = open(sramfile, O_RDONLY);
	if (!f)  {
          printf("Retry...");
          f = open(sramfile, O_RDONLY);
	  if (!f) return -1;
	}
	read(f,ram.sbank, 8192);
	close(f);
	
	return 0;
}


int sram_save()
{
	int f;

	/* If we crash before we ever loaded sram, DO NOT SAVE! */
	if (!mbc.batt || !sramfile || !ram.loaded || !mbc.ramsize)
		return -1;

	f = open(sramfile, O_WRONLY|O_CREAT|O_TRUNC);
	if (!f) {
          printf("Retry...");
          f = open(sramfile, O_WRONLY|O_CREAT|O_TRUNC);
	  if (!f) return -1;
	}
	write(f, ram.sbank, 8192);
	close(f);

	return 0;
}


void state_save(int n)
{
	int f;
	char *name;

	if (n < 0) n = saveslot;
	if (n < 0) n = 0;
	name = malloc(strlen(saveprefix) + 5);
	sprintf(name, "%s.%03d", saveprefix, n);

	if ((f = open(name, O_WRONLY|O_CREAT|O_TRUNC)))
	{
		savestate(f);
		close(f);
	}
	free(name);
}


void state_load(int n)
{
	int f;
	char *name;

	if (n < 0) n = saveslot;
	if (n < 0) n = 0;
	name = malloc(strlen(saveprefix) + 5);
	sprintf(name, "%s.%03d", saveprefix, n);

	if ((f = open(name, O_RDONLY)))
	{
		loadstate(f);
		close(f);
		vram_dirty();
		pal_dirty();
		sound_dirty();
		mem_updatemap();
	}
	free(name);
}

void rtc_save()
{
	int f;
	if (!rtc.batt) return;
	if (!(f = open(rtcfile, O_WRONLY|O_CREAT|O_TRUNC))) return;
	rtc_save_internal(f);
	close(f);
}

void rtc_load()
{
	int f;
	if (!rtc.batt) return;
	if (!(f = open(rtcfile, O_RDONLY))) return;
	rtc_load_internal(f);
	close(f);
}


void loader_unload()
{
	sram_save();
	if (romfile) free(romfile);
	if (sramfile) free(sramfile);
	if (saveprefix) free(saveprefix);
	if (rom.bank) free(rom.bank);
	if (ram.sbank) free(ram.sbank);
	romfile = sramfile = saveprefix = 0;
	rom.bank = 0;
	ram.sbank = 0;
	mbc.type = mbc.romsize = mbc.ramsize = mbc.batt = 0;
}

static char *base(char *s)
{
	char *p;
	p = strrchr(s, '/');
	if (p) return p+1;
	return s;
}

static char *ldup(char *s)
{
	int i;
	char *n, *p;
	p = n = malloc(strlen(s));
	for (i = 0; s[i]; i++) if (isalnum(s[i])) *(p++) = tolower(s[i]);
	*p = 0;
	return n;
}

void cleanup()
{
	sram_save();
	rtc_save();
	/* IDEA - if error, write emergency savestate..? */
}

void loader_init(char *s)
{
	char *name, *p;
	DIR* dir;
        printf("%s - %s\n",s,savedir);
//	sys_checkdir(savedir, 1); /* needs to be writable */
	dir=opendir(savedir);
	if(!dir) {
	  mkdir(savedir,0);
          printf("Savedir created!");
        }         
	else closedir(dir);
	romfile = s;
      printf("Loading rom...\n");
	rom_load();
      printf("Rom loaded!\n");

	vid_settitle(rom.name);

        
	if (savename && *savename)
	{
		if (savename[0] == '-' && savename[1] == 0)
			name = ldup(rom.name);
		else name = strdup(savename);
	}
	else if (romfile && *base(romfile) && strcmp(romfile, "-"))
	{
		name = strdup(base(romfile));
		p = strchr(name, '.');
		if (p) *p = 0;
	}
	else name = ldup(rom.name);
     // name=rom.name;

	saveprefix = malloc(strlen(savedir) + strlen(name) + 2);
	sprintf(saveprefix, "%s/%s", savedir, name);

	sramfile = malloc(strlen(saveprefix) + 5);
	strcpy(sramfile, saveprefix);
	strcat(sramfile, ".sav");

	rtcfile = malloc(strlen(saveprefix) + 5);
	strcpy(rtcfile, saveprefix);
	strcat(rtcfile, ".rtc");
	printf("before sram / rtc load ...");
	sram_load();
	rtc_load();
        printf("       done\n");
//	atexit(cleanup);
}




