#include <uart.h>
    
//
//
//
int main() {
    int c;
    
    while(1) {
        c = uartGetch();
        if (c!=-1) {
            uartOutch(c);    
        }
    }
}
