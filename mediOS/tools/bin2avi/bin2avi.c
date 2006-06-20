/*

 All files in this archive are subject to the GNU General Public License.
 See the file COPYING in the source tree root for full license agreement.
 This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 KIND, either express of implied.

 Creates a .avi ready to use on the Gmini402 from a binary file and a template

 Date:     11/09/2005
 Author:   GliGli

*/

#include <stdio.h>
#include <stdlib.h>

#define BINFILE_OFFSET 0x249E00 // 0xF00000 sdram load offset (fw 1.3.11)

int main(int argc,char *argv[]){

  // little help if bad parameters
  if (argc<4){
    printf("%s : Creates a .avi ready to use on the Gmini402 from a binary file and a template\n",argv[0]);
    printf("By GliGli\n");
    printf("usage: %s binfile templatefile outfile [offset]",argv[0]);

    return -1;
  }

  FILE * in;
  FILE * out;
  FILE * tpl;
  unsigned long offset=BINFILE_OFFSET;

  if (argc>=5){
    offset=atoi(argv[4]);
    printf("Using offset 0x%0.8x(%d)\n",offset,offset);
  }

  // opening the files
  in=fopen(argv[1],"r");
  if(!in){
    perror("Error opening binfile");
    return -1;
  }

  tpl=fopen(argv[2],"r");
  if(!tpl){
    perror("Error opening templatefile");

    fclose(in);
    return -1;
  }

  out=fopen(argv[3],"w+");
  if(!out){
    perror("Error opening outfile");

    fclose(in);
    fclose(tpl);
    return -1;
  }

  int insize;
  int tplsize;
  char *inbuf;
  char *tplbuf;

  // get infile/tplfile file size
  fseek(in,0,SEEK_END);
  insize=ftell(in);
  rewind(in);

  fseek(tpl,0,SEEK_END);
  tplsize=ftell(tpl);
  rewind(tpl);

  // alloc buffers
  inbuf=malloc(insize);
  tplbuf=malloc(tplsize);

  // read infile/tplfile
  fread(inbuf,insize,1,in);
  fread(tplbuf,tplsize,1,tpl);

  // write tplfile data
  fwrite(tplbuf,tplsize,1,out);

  // write infile data
  fseek(out,offset,SEEK_SET);
  fwrite(inbuf,insize,1,out);

  free(inbuf);
  free(tplbuf);

  fclose(in);
  fclose(tpl);
  fclose(out);

  return 0;
}
