#include <api.h>
#include <fs_io.h>

int test_fct(int var);

int _start(int argc,char**argv)
{
    int fd;
   
    //char c;
    printf("In test\n");
    fd=fopen("/avlo.cfg", O_RDONLY);
    printf("filesize : %d\n", filesize(fd));
    /*while(fread(fd,&c,1)==1 && c != 0)
        printf("%c",c);*/
    printf("eof\n");
    fclose(fd);
    printf("before call\n");
    test_fct(3);
    printf("Out test\n");
    return 0;
}

__attribute__ ((section(".core"))) int test_fct(int var)
{
    *(unsigned char*)0x030300 = 'h';
    *(unsigned char*)0x030300 = 'h';
    *(unsigned char*)0x030300 = 'h';
    *(unsigned char*)0x030300 = 'h';
    printf("in test_fct\n");
    return 0;
}

