#include <libavos.h>

int main() {
  int i=0, x=0, y=0;
  lcdInitT();
  for (y=0;y<240;y+=15) {
    for (x=0;x<320;x+=20) {
      lcdRectfST(x, y, 20, 15, i++);
    }
  }
  while(1)
    ;
}
