#include "medios.h"

int access(char *file, int mode)
{
	int test_fp;

	test_fp = open(file, O_RDONLY);
	if ( test_fp >= 0 ) {
		close(test_fp);
		return(0);
	}
	return(-1);
}

int fprintf(int f,const char * s,...){
  switch(f){
    case stderr:
      printf("STDERR: %s",s);
      break;
    case stdout:
      printf("STDOUT: %s",s);
      break;
    default:
      printf("FILE %d: %s",f,s);
      break;
  }

  return 0;
};

int fscanf(int f,const char * s,...){
  return 0;
};

void * getenv(const char * s){
  return NULL;
}

void setbuf(int f,void * p){
}

void getchar(){
}

