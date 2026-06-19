#include "buttons.h"


int waitForKeyPress()
{
	int key;
	while((key=buttonsGetStatusT()) == noButton) ;
	return key;
}

void waitForKeyReleased(int loopCount)
{
	while(buttonsGetStatusT() != noButton && loopCount >0)
		loopCount--;
}
