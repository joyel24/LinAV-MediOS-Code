#include <stdio.h>
#include <stdlib.h>

#include "cops.h"
#include "avevents.h"
#include "font.h"

#define WHITE        0
#define BLACK        1
#define GRAY         21
#define BLUE         32
#define RED          77


struct client_operations * cops;

int main(int argc,char * * argv)
{
	REGISTER(cops,NULL,0);
	
	cops->fillRect(WHITE,5, 15 , 320,225);
	cops->putS(BLACK, BLUE,10, 40, "string 1");
	cops->setFont(STD8X13);
	cops->putS(BLACK, BLUE,10, 60, "string 2");
	cops->setFont(STD6X9);
	cops->putS(BLACK, BLUE,10, 80, "string 3");
	
	while(1);
	return 0;
}