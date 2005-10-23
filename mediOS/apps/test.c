#include <api.h>
#include <fs_io.h>

int _start(int argc,char**argv)
{
    int fd;
    char c;
    printf("In test\n");
    fd=fopen("/avlo.cfg", O_RDONLY);
    printf("filesize : %d\n", filesize(fd));
    while(fread(fd,&c,1)==1 && c != 0)
        printf("%c",c);
    //fclose(fd);
    printf("Out test\n");
    return 0;
}
