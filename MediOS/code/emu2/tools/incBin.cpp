/* 
*   find_last_data.cpp
*
*   AV3XX emulator - tools
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINE_LENGTH 10

int main(int argc, char ** argv)
{
    FILE * file1,*file2;

    int c;
    int pos=0;
    
    if(argc>2)
    {
        file1 = fopen(argv[1],"rb");
        if(!file1)
        {
            printf("Error: opening %s for reading\n",argv[1]);
            return 0;
        }
        
        file2 = fopen(argv[2],"w");
        
        if(!file2)
        {
            printf("Error: opening %s for writting\n",argv[2]);
            return 0;
        }
        
        fprintf(file2,"/*\n* %s bytes array\n*/\n\n",argv[3]);
        
        fprintf(file2,"char %s[] = {\n",argv[3]);
        
        while((c = fgetc(file1)) != EOF)
        {    
            pos++;         
            fprintf(file2,"0x%02x, ",c);
            if((pos % LINE_LENGTH)==0)
                fprintf(file2,"\n");
        }
        fprintf(file2,"\n};\n\n");
        fclose(file1);
        fclose(file2);
    }

}
