#include <uart.h>
#include <string.h>    

//
//
//
int main() {
    int c;
    char p[] = "val = [xx] [x]\n";

    while(1) {
        c = uartGetch();
        if (c!=-1) {
            stringPutHex(p + 7, c, 2);
            p[12] = (char) c;
            uartOuts(p);
        }
    }
}
