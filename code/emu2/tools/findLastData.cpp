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

int main(int argc, char ** argv)
{
    FILE * file;
    char data[2];
    int pos = 0;
    int max_pos = 0;
    
    if(argc>1)
    {
        file = fopen(argv[1],"rb");
        if (!file)
            printf("Error: opening: %s\n",argv[1]);
        else
        {
            while(fread(&data,1,1,file)==1)
            {
                if(data[0])
                    max_pos=pos;
                pos++;                
            }
            fclose(file);
            printf("Read file %s, %d car last data car: %d\n",argv[0],pos,max_pos);        
        }  
    }

}
