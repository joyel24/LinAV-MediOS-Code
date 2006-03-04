#include "fft.h"
extern "C" {
#include <sys_def/section_types.h>
#include <sys_def/timer.h>
#include <stdlib.h>
  //#include <stdio.h>
#include <sys_def/file.h>
#include <kernel/file.h>
#include <kernel/buttons.h>
#include <kernel/ata.h>

#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/lcd.h>

#include <kernel/timer.h>

#include "dspshared.h"
#undef	__USE_XOPEN
#include "aic23.h"

#include "high_resolution_timer.h"
#include "profile.h"

  int app_main(int argc, char * * argv);

#include <kernel/pipes.h>
int uart_in(unsigned char * data,int uartNum);
extern struct pipe * UART_0_Pipe;

}

#include "spc/libspc/libspc.h"
#include "spc/libspc/port.h"

#include "browser.h"

static SPC_Config spc_config = {
  //32000, /* frequency (SPC hardware frequency is 32Khz) */
  //16000,
  //44100,
  FRQ,
  16,    /* bits per sample */
  2,     /* number of channels */
  1,     /* is interpolation ? */
  1,     /* is echo ? */
};

/** Current SPC info */
static SPC_ID666 spcinfo;

/** PCM buffer */
//typedef uint32_t uint32;
static int32 *buf;
static int buf_size;
static int buf_cnt;
static int ready;

static struct tmr_s ptimer;
static volatile int player_running;

extern "C" {
#include <sys_def/string.h>
#include <sys_def/graphics.h>
#include <evt.h>
#include <buttons.h>
#include <kernel/osd.h>
#include <kernel/graphics.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>
  //#include "gui/file_browser.h"
#include "gui/icons.h"
#include "gui/common_def.h"
#define LCD_WIDTH 220
#define LCD_HEIGHT 176
#define X_OFFSET 0x09
#define Y_OFFSET 0x09
extern int gui_pal[256][3];
}

#define REG (volatile unsigned short *)
#define REG_ARM_CLOCK     (REG 0x30880)
void overclockARM(int rate){
  if(rate==0){
    *REG_ARM_CLOCK=0x80E1; // default ARM clock
  }else{
    *REG_ARM_CLOCK=(0x8070+(rate<<4));
  }
}

#if 1
extern unsigned int cpu_stats[256];
extern unsigned int cpu_stats2[256];
static int idx[256];

int cmp_idx(const void * pa, const void * pb)
{
  const int * a = (int *) pa;
  const int * b = (int *) pb;
  
  if (cpu_stats2[*a] == cpu_stats2[*b]) return cpu_stats[*a] <= cpu_stats[*b];
  
  return cpu_stats2[*a] < cpu_stats2[*b];
}

extern "C" void qsort(void *base, size_t nmemb, size_t size,
		      int (*compare)(const void *, const void *));
#endif

void do_cpu_stats()
{
#if 1
  char s[128];
  int i;
  for (i=0; i<256; i++) idx[i] = i;
  qsort(idx, 256, sizeof(int), cmp_idx);
  for (i=0; i<256; i++) {
    int j = idx[255-i];
    printf("%2x:%8x%8x\n", j, cpu_stats2[j], cpu_stats[j]);
  }
  while (!(btn_readState() & 4));
#endif
}

static void clean_spc_info(void)
{
  memset(&spcinfo, 0, sizeof(spcinfo));
}

static int init()
{
  printf("spc : Init\n");
  ready = 0;
  buf = 0;
  buf_size = 0;
  buf_cnt = 0;
  clean_spc_info();

  return 0;
}

static int stop(void)
{
  if (ready) {
    SPC_close();
  }
  if (buf) {
    //free(buf);
  }
  buf = 0;
  buf_size = 0;
  buf_cnt = 0;
  clean_spc_info();
  ready = 0;

  return 0;
}
  
static int shutdown()
{
  stop();
  return 0;
}

__IRAM_DATA static int sbuf[128];
static int start(const char *fn, int track)
{

  stop();

  buf_size = SPC_init(&spc_config);
  if (buf_size <= 0 || (buf_size&3)) {
    goto error;
  }
  buf = sbuf;//(int32*)malloc(buf_size);
  if (!buf) {
    goto error;
  }
  buf_size >>= 2;

  printf("SPC buffer size = %d\n", buf_size);

  if (! SPC_load(fn, &spcinfo)) {
    goto error;
  }

//   //playa_info_bps    (info, 0);
//   playa_info_desc   (info, "SNES music");
//   playa_info_frq    (info, spc_config.sampling_rate);
//   playa_info_bits   (info, spc_config.resolution);
//   playa_info_stereo (info, spc_config.channels-1);
//   playa_info_time   (info, spcinfo.playtime * 1000);
//   //  spc_config.is_interpolation = 1;

  buf_cnt = buf_size;
  ready = 1;

  printf("SPC init ok\n");

  return 0;

 error:
  stop();
  return -1;
}

__IRAM_DATA static int dsp_write;

static int fifo_room()
{
  return (dsp_com->readPos - dsp_write - 2) & (DSP_BUFFER_SIZE-1);
}

extern "C" __IRAM_CODE int fifo_readbak(int * buf, int count);
__IRAM_CODE int fifo_readbak(int * buf, int count)
{
  int w = dsp_com->readPos;
  int ini = count;
  while (count > 0) {
    *buf++ = dsp_com->buffer[w] | (((int32_t) dsp_com->buffer[w+1]) << 16);
    w = (w + 2) & (DSP_BUFFER_SIZE-1);
    count--;
  }
  return ini-count;
}

__IRAM_CODE static int fifo_write(int * buf, int count)
{
  int ini = count;
  while ( count > 0 ) {
    int count2;
    int w = dsp_write;

    count2 = (dsp_com->readPos - w - 2) & (DSP_BUFFER_SIZE-1);
    if (!count2)
      break;

    count2 >>= 1;
    if (count2 > count) 
      count2 = count;
    count -= count2;

    while (count2 > 0) {
      unsigned int a = *buf++;
      dsp_com->buffer[w] = a;
      dsp_com->buffer[w + 1] = a>>16;
      w = (w + 2) & (DSP_BUFFER_SIZE-1);
      count2--;
    }
    dsp_com->writePos = w;
    dsp_write = w;
  }
  return ini - count;
}

__IRAM_CODE static int decoder()
{
  int n;


  if (!ready) {
    return -1;
  }

  if (buf_cnt >= buf_size) {
    SPC_update((unsigned char *)buf);
    buf_cnt = 0;
  }

  prof(PROF_FIFO);

  n = fifo_write( (int *)buf + buf_cnt, buf_size - buf_cnt);
  if (n > 0) {
    buf_cnt += n;
  } else if (n < 0) {
    return -1;
  }

  return buf_cnt < buf_size;
}


int app_main(int argc, char * * argv)
{
  int newbutton, oldbutton;
  int timer_prof, timer_disp, tick;

  init_high_resolution_timer();
  
  // dis irq
//   asm(
//     "mrs r0,cpsr;"
//     "orr r0,r0,#0x80;"
//     "msr cpsr_cf,r0;"
//   );

  printf("Hello World !\n");

//   ata_sleep();
//   ata_powerDownHD();

  initDSP();

//   asm(
//     "mrs r0,cpsr;"
//     "orr r0,r0,#0x80;"
//     "msr cpsr_cf,r0;"
//   );

  printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %d  Malloc start: %08x, size: %d\n",get_sp(),
	 (unsigned int)&_end_kernel,
	 (unsigned int)&_iram_end - (unsigned int)&_iram_start,
	 (unsigned int)MALLOC_START,
	 (unsigned int)MALLOC_SIZE/1024);



//   {
//     int i;
//     for (i=0; i<10; i) {
//       unsigned char c;
//       uart_in(&c, 0);
//       if (c)
// 	printf("%c\n", c);
//     }
//   }


  {
    int i;
    int max = 0, min = 0x10000;
    for (i=0; i<1000000; i++) {
      int a = TMR_GET_CNT(TMR0);
      if (a>max) max = a;
      if (a<min) min = a;
    }
    printf("timer %d %d %d %d\n", min, max, TMR_GET_SCAL(TMR0), TMR_GET_DIV(TMR0));
  }

  fftbands_t * bands = fft_create_bands(SCREEN_REAL_WIDTH, 0);

  init();

  tmr_setup(&ptimer, "player timer");

//   ata_sleep();
//   ata_powerDownHD();
  overclockARM(2);

  char fn[PATHLEN] = "/spc/xmen/x_men_comics.spc";
  osd_setEntirePalette(gui_pal,256);
  gfx_planeSetSize(BMAP1,LCD_WIDTH,LCD_HEIGHT,8);
  gfx_planeSetPos(BMAP1,X_OFFSET,Y_OFFSET);
  iniIcon();
  gfx_openGraphics();
  gfx_clearScreen(COLOR_WHITE);
  gfx_fontSet(STD6X9);
  //goto first;
  spc_ini_file_browser("/spc", 1);
  gfx_fillRect(0, 0, 0, SCREEN_REAL_WIDTH, SCREEN_HEIGHT);
 again:
  {
//     char * p = fn + strlen(fn);
//     while (*p != '/') *p--;
//     if (p > fn) p[0] = 0;
//     else p[1] = 0;


    char * newfn = spc_browse();
    
    if (btn_readState() & 8)
      goto finished;

    if (!newfn) goto finished;
    if (!strcmp(newfn, fn)) goto same;
    strcpy(fn, newfn);
  }

 first:
  if (player_running) {
    player_running = 0;
    printf("waiting player stop\n");
    while (!player_running);
    player_running = 0;

    stop();
  }

  start(fn, 0);

  extern void player();
  ptimer.action = player;
  player_running = 1;
  ptimer.trigger = 1;

 same:
  while (btn_readState());

  tick = tmr_getTick();
  timer_prof = tick + 100;
  timer_disp = tick + 2;
  static int old[SCREEN_REAL_WIDTH];
  static int old2[SCREEN_REAL_WIDTH];
  memset(old, 0, sizeof(old));
  memset(old2, 0, sizeof(old));
  gfx_fillRect(0, 0, 10, SCREEN_REAL_WIDTH, SCREEN_HEIGHT - 10);
  while (!btn_readState()) {
    tick = tmr_getTick();
    if (timer_prof <= tick) {
      timer_prof = tick+100;
      //prof_print();
    }
    if (timer_disp <= tick) {
      timer_disp = tick+2;
      if (btn_readState() & 8) goto finished;
      
      prof(PROF_FFT);
      fft_queue();
      prof(PROF_FFT2);
      fft_fill_bands(bands);

      prof(PROF_DISP);
      int i;
#define H (SCREEN_HEIGHT-10)
      for (i=0; i<SCREEN_REAL_WIDTH; i++) {
	int y, oldy;

	y = old[i];
	oldy = old2[i];
	old2[i] = y;
	if (y > oldy) {
// 	  gfx_fillRect(COLOR_WHITE, i, 10+H/2-y, 1, y-oldy);
// 	  gfx_fillRect(COLOR_WHITE, i, 10+H/2+oldy, 1, y-oldy);
	} else {
	  gfx_fillRect(0, i, 10+H/2-oldy, 1, oldy-y);
	  gfx_fillRect(0, i, 10+H/2+y, 1, oldy-y);
	}

	y = bands->band[i].v / (0x800 / 64);
	if (y>=H/2) y = H/2 - 1;
	oldy = old[i];
	old[i] = y;
	if (y > oldy) {
	  gfx_fillRect(COLOR_WHITE, i, 10+H/2-y, 1, y-oldy);
	  gfx_fillRect(COLOR_WHITE, i, 10+H/2+oldy, 1, y-oldy);
	} else {
	  gfx_fillRect(COLOR_RED, i, 10+H/2-oldy, 1, oldy-y);
	  gfx_fillRect(COLOR_RED, i, 10+H/2+y, 1, oldy-y);
	}
      }
      prof(PROF_MISC);

//       static int old[SCREEN_REAL_WIDTH];
//       static int old2[SCREEN_REAL_WIDTH];
//       int i;
//       int offs = *DSP_READ/* - SCREEN_REAL_WIDTH*2 - 2*/;
//       for (i=0; i<SCREEN_REAL_WIDTH; i++) {
// 	gfx_drawPixel(0, i, old2[i]+10);
// 	old2[i] = old[i];
// 	gfx_drawPixel(1, i, old[i]+10);
// 	int y = (DSP_BUFFER[offs & (DSP_BUFFER_SIZE-1)] ^ 0x8000)/* + (DSP_BUFFER[(offs+1) & (DSP_BUFFER_SIZE-1)] ^ 0x8000)*/;
// 	offs += 2;

// 	y = y/(32768/64);
// 	gfx_drawPixel(COLOR_WHITE, i, y+10);
// 	old[i] = y;
//       }
    }
  }
  while (btn_readState());
#if 0
  while (!(btn_readState() & 1)) {
    static short test;
    static short OLD_READ;
    
    //printf("dsp %d %d %d %d\n", *DSP_READ, *DSP_WRITE, *DSP_TEST, *DSP_READY);
    
#if 0
    if ((btn_readState() & 2)) goto force;

    if (*DSP_READ == OLD_READ) {
      int i;
      for (i=0; i<10000; i++)
	if (*DSP_READ != OLD_READ) goto ok;

    force:
      test++;
      outw(test,CPLD_PORT3);
      printf("%d\n", test);
      for (i=0; i<1000000; i++) (*DSP_TEST)++;
    } else {
    ok:
      OLD_READ = *DSP_READ;
    }
#else
    if ((btn_readState() & 8)) goto finished;
#endif

  }
#endif

  goto again;

 finished:
  overclockARM(0);
  //ata_powerUpHD();

  ptimer.trigger = 0;

  overclockARM(0);

  do_cpu_stats();

  dsp_com->stop = 1; //stop the DSP
  aic23Shutdown();   //stop the codec
  
  return 0;
}


__IRAM_DATA static int loop_limit;
void player()
{
//   pipe * pPipe = UART_0_Pipe;
//   while (pPipe->nIN != pPipe->nOUT) {
//     char c;
//     pipeRead(pPipe, &c, 1);
//     printf("%c", c);
//   }

  static int entered;

  if (entered) {
    loop_limit += 10;
    ptimer.trigger = 1;
    return;
  }

  if (!player_running) {
    player_running = 1;
    return;
  }

  entered = 1;
  ptimer.trigger = 1;

  int sprof = prof_curid();

  // enable irqs
  asm(
    "mrs r0,cpsr;"
    "bic r0,r0,#0x80;"
    "msr cpsr_cf,r0;"
  );

  int i;
  loop_limit = 10;
  int room = fifo_room();
//   if (room > DSP_BUFFER_SIZE/4)
//     n = 40;
  for (i=0; i<loop_limit; i++) {
    if (decoder())
      break;
  }

  static int over8;
  if (!over8)
    prof_print();
  over8 = (over8+1)&7;
  prof(PROF_DISP);
  // fifo bar
  {
    //int x = (DSP_BUFFER_SIZE-1-room) * (SCREEN_REAL_WIDTH-1) / (DSP_BUFFER_SIZE-1);
    extern int free_cpu;
    int x = (1024-free_cpu)*(SCREEN_REAL_WIDTH-1)/1024;
    static int oldx;
    if (x > oldx)
      gfx_fillRect(1, oldx, 0, x-oldx, 10);
    else
      gfx_fillRect(0, x, 0, oldx-x, 10);
    oldx = x;
  }

  prof(sprof);

  entered = 0;
}
