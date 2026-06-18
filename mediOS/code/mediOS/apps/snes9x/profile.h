#ifdef __cplusplus
extern "C" {
#endif

#define PROF0_NAME "fifo"
#define PROF_FIFO 0

#define PROF1_NAME "misc"
#define PROF_MISC 1

#define PROF2_NAME "cpu"
#define PROF_CPU 2

#define PROF3_NAME "apu"
#define PROF_APU 3

#define PROF4_NAME "apu2"
#define PROF_APU2 4

#define PROF5_NAME "fft"
#define PROF_FFT 5

#define PROF6_NAME "fft2"
#define PROF_FFT2 6

#define PROF7_NAME "disp"
#define PROF_DISP 7

#define PROF8_NAME "apu"
#define PROF9_NAME "apu"

void prof(int id);
int prof_curid();
void prof_print();
void prof_dsp_print();

#ifdef __cplusplus
}
#endif
