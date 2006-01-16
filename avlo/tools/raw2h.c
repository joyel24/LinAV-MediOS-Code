
#include <stdio.h>
#include <stdlib.h>


#include "../include/avlo_cfg.h"
#include "parse_cfg.h"


#define ITEM_TEST(NAME,VAR) if(!strcmp(item,NAME)) {cfg->VAR=atoi(value); nb++;} 

extern char item_buff[MAX_TOKEN+1];
extern char value_buff[MAX_TOKEN+1];

void do_parse(struct avlo_cfg * cfg)
{
    char *item=item_buff;
    char *value=value_buff;
    int pos;
    int nb=0;
    while (1) 
    {
        if (!nxt_cfg(item,value)) break;
        
        ITEM_TEST("menu_x",menu_x)
        else ITEM_TEST("menu_y",menu_y)
        else ITEM_TEST("status_x",status_x)
        else ITEM_TEST("status_y",status_y)
        else ITEM_TEST("bar_x",bar_x)
        else ITEM_TEST("bar_y",bar_y)
        else ITEM_TEST("bar_w",bar_w)
        else ITEM_TEST("bar_h",bar_h)
        else ITEM_TEST("usb_x",usb_x)
        else ITEM_TEST("usb_y",usb_y)
        else ITEM_TEST("bat_x",bat_x)
        else ITEM_TEST("bat_y",bat_y)
        else
            printf("unknown item type: %s on line %d \n",item,line_num);  
    }
    printf("found %d param\n",nb); 
}

struct avlo_cfg cfg;

int main(int argc, char* argv[]) {
    int w,h,i,j,c,ept;
    int stop=0;
    int count;
    int binOut;
    unsigned char rgb[3];
    int r,g,b,y,cr,cb;
    
    FILE * infile;
    FILE * outfile; 

    w=320;h=240;
    
    char * ptr = (char*) &cfg;
    
    if(argc != 4)
    {
        fprintf(stderr,"Info: %s source destination layout\n",argv[0]);
        exit(1);
    }   

    infile = fopen(argv[1], "rb");

    if (!infile)
    {
        printf("Can't open file '%s' for reading!\n", argv[1]);
        exit(1);
    }
    
    outfile = fopen(argv[2], "wb");
    
    if (!outfile)
    {
        printf("Can't open file '%s' for writing!\n", argv[2]);
        fclose(infile);
        exit(1);
    }
    
    /* reading layout */
    if(!file_open(argv[3]))
    {
        printf("Can't open file '%s' for reading!\n", argv[3]);
        fclose(infile);
        fclose(outfile);
        exit(1);
    }
    memcpy(&cfg,&default_cfg,sizeof(struct avlo_cfg));
    do_parse(&cfg);
    file_close();
    
    for(i=0;i<h && !stop;i++)
    {
        count=0;
        for(j=0;j<w;j++)
        {
            if (feof(infile)) {stop=1;break;}
            c = fread(rgb, 1, 3, infile);
            if (c == 0) {stop=1;break;}
            if (c != 3) {fprintf(stderr,"\nMismatch!\n\n");stop=1;break;}
            r  = rgb[0];
            g  = rgb[1];
            b  = rgb[2];
            y  =  299*r +  587*g + 114*b;
            cb = -169*r + -331*g + 500*b + 128000;
            cr =  500*r -  419*g -  81*b + 128000;
            
            y = (y+500)/1000;
            cb = (cb+500)/1000;
            cr = (cr+500)/1000;
    
            if (y < 0) y = 0;
            if (y > 255) y = 255;
            if (cb < 0) cb = 0;
            if (cb > 255) cb = 255;
            if (cr < 0) cr = 0;
            if (cr > 255) cr = 255;
            
            
            fputc(cb,outfile);
            fputc(y,outfile);
            fputc(cr,outfile);
            fputc(0x80,outfile);    
            count++;
        }
        
        ept=32-w%32;
        // printf("%d:%d\n",i,ept);
        if(w%32)
        {
        for(ept=32-w%32;ept>0;ept--)
        {
            fputc(0x80,outfile);
            fputc(0xFF,outfile);
            fputc(0x80,outfile);
            fputc(0x80,outfile);
        }
        }
            
    }
       
    /* adding cfg struct */
    fwrite(ptr,1,sizeof(struct avlo_cfg),outfile);
    fclose(infile);
    fclose(outfile);
    
    return 0;
}
