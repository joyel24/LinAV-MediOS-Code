#include <libavos.h>

int main() {
  int i=0;
  lcdInitT();
  for( i=0; i<100; i++ ) {
    terminalPutsT( "Hello world!  This is the first C program running on an Archos, by awksedgrep!!!  Greets to Dogger!!!\n" );
  }
  while(1)
    ;
}
