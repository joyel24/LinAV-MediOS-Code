#include <uart.h>
#include <string.h>    

//
//
//
int main() {
    int c;
    char p[] = "val = [xx] [x]\n";

    while(1) {
        c = uartGetchA();
        if (c!=-1) {
            stringPutHexA(p + 7, c, 2);
            p[12] = (char) c;
            uartOutsA(p);
        }
    }
}
