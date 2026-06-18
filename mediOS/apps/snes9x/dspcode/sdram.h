
// direction : 1 dsp-->sdram, 0 sdram-->dsp
void dma_dsp2sdram(void * dsp_addr, long sdram_addr, short length, short direction);

int dma_pending();

typedef struct {
  void * dsp_addr;
  long sdram_addr;
  unsigned short length;
  short direction;
} dma_command_t;

// enqueue a dma request, return id of the request or -1 if the dma queue is full
int dma_enqueue(void * dsp_addr, long sdram_addr, short length, short direction);

// test given request is completed
// when it returns true , the id is released
int dma_complete(int id);
