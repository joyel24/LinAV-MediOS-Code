#include <stdio.h>
#include <stdlib.h>

#include "../avwm.h"

struct client_operations * cops;

int main(int argc,char * * argv)
{
	printf("argc=%d, argv0:%s argv1:%s\n",argc,argv[0],argv[1]);
	cops=atoi(argv[1]);
	cops->putS(77, 1,12, 12, "hello world");
	printf("in child: %x\n",cops);
	while(1);
	return 0;
}