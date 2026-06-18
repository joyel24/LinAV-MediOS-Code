#include <sys_def/types.h>
#include <sys_def/section_types.h>
#include <sys_def/timer.h>
#include <kernel/io.h>

#include "profile.h"

#include "dspshared.h"

#define uint64_t uint32_t

extern unsigned long tick __IRAM_DATA; // defined in kernel/core/timer.c
static uint64_t timer_div __IRAM_DATA;
__IRAM_CODE uint64_t get_high_resolution_timer()
{
  return tick * timer_div + TMR_GET_CNT(TMR0);
}

void init_high_resolution_timer()
{
  timer_div = TMR_GET_DIV(TMR0);
}

uint64_t counts[32];
uint64_t last;
int cur_id;
int max_id;
int free_cpu;

char * names[32] = {
  PROF0_NAME,
  PROF1_NAME,
  PROF2_NAME,
  PROF3_NAME,
  PROF4_NAME,
  PROF5_NAME,
  PROF6_NAME,
  PROF7_NAME,
  PROF8_NAME,
  PROF9_NAME,
};

int prof_curid()
{
  return cur_id;
}

void prof(int id)
{
  uint64_t a = get_high_resolution_timer();
  counts[cur_id] += a - last;

  last = a;
  cur_id = id;
  if (max_id < id) max_id = id;
}

void prof_print()
{
#ifdef PROFILE
  int i;
  uint64_t total = 0;
  for (i=0; i<=max_id; i++)
    total += counts[i];
  //free_cpu = (counts[PROF_MISC] + counts[PROF_FFT] + counts[PROF_FFT2] + counts[PROF_DISP])*1024/total;
  for (i=0; i<=max_id; i++) {
    //printf("%10s: %10d %5d\n", names[i], counts[i], (int)(counts[i]*1000/total));
    counts[i] = 0;
  }
  //printf("\n");
#endif
}

void prof_dsp_print()
{
#ifdef PROFILE
  int i;
  uint32_t total = 0;
  for (i=0; i<8; i++)
    total += dsp_com->prof[i];
  free_cpu = (dsp_com->prof[5])*1024/total;
  for (i=0; i<8; i++) {
    printf("%2d: %10d %5d\n", i, dsp_com->prof[i], (int)(dsp_com->prof[i]*1000/total));
    dsp_com->prof[i] = 0;
  }
#ifdef PROFILE_OPCODES
  for (i=8; i<256+8; i++)
    dsp_com->prof[i] = 0;
#endif
  printf("\n");
#endif
}
