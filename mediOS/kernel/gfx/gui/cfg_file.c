/*
* kernel/gfx/gui/cfg_file.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/cfg_file.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/stdfs.h>

#include <sys_def/stddef.h>
#include <sys_def/string.h>

typedef struct {
    bool dummy;
    bool deleted;
    char * name;
    char * value;
} CFG_ITEM;

static CFG_ITEM * cfg_items = NULL;
static int cfg_itemCount = 0;

static int cfg_currentItem = 0;

static CFG_ITEM * cfg_getItem(char * name){
    int i;

    for(i=0;i<cfg_itemCount;++i){

        if(!cfg_items[i].dummy && !cfg_items[i].deleted &&!strcmp(cfg_items[i].name,name)){

            return &cfg_items[i];
        }
    }

    // item not found
    return NULL;
}

static CFG_ITEM * cfg_addItem(){
    cfg_itemCount++;

    if(cfg_items==NULL){
        printk("[cfg] cfg_items not allocated !\n");
        return NULL;
    }

    cfg_items=realloc(cfg_items,cfg_itemCount*sizeof(CFG_ITEM));

    return &cfg_items[cfg_itemCount-1];
}

void cfg_clear(){
    int i;

    if (cfg_items!=NULL){

        // clear items data
        for(i=0;i<cfg_itemCount;++i){
            free(cfg_items[i].name);
            if (!cfg_items[i].dummy) free(cfg_items[i].value);
        }

        free(cfg_items);
        cfg_items=NULL;
    }

    cfg_itemCount=0;
}

void cfg_newFile(){
    cfg_clear();
    cfg_items=malloc(0);
}

bool cfg_readFile(char * filename){
    int f;
    char * data;
    char prev;
    int size;
    int namebeg,nameend,valbeg,valend;
    int i;
    int ii;
    CFG_ITEM * item;

    cfg_clear();

    // open the file and read its content into a temp buffer

    f=open(filename,O_RDONLY);

    if (f<0){
        printk("[cfg] can't open file!\n");
        return false;
    }

    size=filesize(f);

    data=malloc(size);

    if (read(f,data,size)!=size){
        printk("[cfg] read error!\n");
        free(data);
        close(f);
        return false;
    }

    close(f);

    // read the data once to count how many items there is and alloc cfg_items

    prev=0;
    for(i=0;i<size;++i){
        if (prev=='\n' || prev==0) cfg_itemCount++;
        prev=data[i];
    }

    cfg_items=malloc(cfg_itemCount*sizeof(CFG_ITEM));

    // read again and fill cfg_items

    i=0;
    ii=0;
    do{
        item=&cfg_items[ii];
        item->dummy=true;
        item->deleted=false;
        item->name=NULL;
        item->value=NULL;

        namebeg=nameend=valbeg=valend=i;

        // parse a line
        while(i<size && data[i]!='\r' && data[i]!='\n'){

            if(data[i]=='=' && valbeg==nameend){ // first '=' on a line?
                item->dummy=data[namebeg]=='#'; // dummy item if no '=' or line is a comment

                nameend=i;
                valbeg=valend=i+1;
            }

            i++;
        };

        if (item->dummy){ // dummy item -> the whole line goes into the 'name' field
            nameend=i;
        }else{
            valend=i;
        }

        // copy name
        item->name=malloc(nameend-namebeg+1);
        memset(item->name,0,nameend-namebeg+1);
        strncpy(item->name,&data[namebeg],nameend-namebeg);

        // copy value
        if(!item->dummy){
            item->value=malloc(valend-valbeg+1);
            memset(item->value,0,valend-valbeg+1);
            strncpy(item->value,&data[valbeg],valend-valbeg);
        }

        // handle CR+LF
        if (i<size-1 && data[i]=='\r' && data[i+1]=='\n'){
            i++;
        }

        i++;
        ii++;
    }while(i<size);

    return true;
}

bool cfg_writeFile(char * filename){
    int f;
    int i;
    CFG_ITEM * item;
    char * line=malloc(256);

    // open the file and write back items

    f=open(filename,O_RDWR | O_CREAT);

    if (f<0){
        printk("[cfg] can't open file!\n");
        return false;
    }

    for(i=0;i<cfg_itemCount;++i){
        item=&cfg_items[i];

        if(!item->deleted){

            // build line
            strcpy(line,item->name);
            if(!item->dummy){ // dummy items don't have value
                strcat(line,"=");
                strcat(line,item->value);
            }
            strcat(line,"\r\n");

            // write line
            if (write(f,line,strlen(line))!=strlen(line)){
                printk("[cfg] write error!\n");
                close(f);
                free(line);
                return false;
            }
        }
    }

    // truncate file
    truncate(f,lseek(f,0,SEEK_CUR));

    close(f);
    free(line);

    return true;
}

void cfg_rewindItems(){
    cfg_currentItem=0;
}

bool cfg_nextItem(char * * name,char * * value){

    if(cfg_itemCount==0) return false;

    // find next valid item
    do{
        cfg_currentItem++;
    }while(cfg_items[cfg_currentItem].dummy);

    if(cfg_currentItem<cfg_itemCount){
        *name=cfg_items[cfg_currentItem].name;
        *value=cfg_items[cfg_currentItem].value;

        return true;
    };

    return false;
}

bool cfg_itemExists(char * name){

    return cfg_getItem(name)!=NULL;
}

char * cfg_readString(char * name){
    CFG_ITEM * item;

    item=cfg_getItem(name);

    if(item!=NULL){

        return item->value;
    };

    return "";
}

int cfg_readInt(char * name){

    return atoi(cfg_readString(name));
}

bool cfg_readBool(char * name){

    return (cfg_readInt(name))?true:false;
}

void cfg_writeString(char * name,char * value){
    CFG_ITEM * item;

    item=cfg_getItem(name);

    if(item!=NULL){

        item->value=realloc(item->value,strlen(value)+1);
        strcpy(item->value,value);
    }else{

        // item not found -> create new item
        item=cfg_addItem();

        item->dummy=false;
        item->deleted=false;
        item->name=malloc(strlen(name)+1);
        strcpy(item->name,name);
        item->value=malloc(strlen(value)+1);
        strcpy(item->value,value);
    }
}

void cfg_writeInt(char * name,int value){
    char sv[12];

    sprintf(sv,"%d",value);

    cfg_writeString(name,sv);
}

void cfg_writeBool(char * name,bool value){

    if(value){

        cfg_writeString(name,"1");
    }else{

        cfg_writeString(name,"0");
    }
}

void cfg_addDummyLine(char * text){
    CFG_ITEM * item;

    item=cfg_addItem();

    item->dummy=true;
    item->deleted=false;
    item->value=NULL;
    item->name=malloc(strlen(text)+1);
    strcpy(item->name,text);
}

bool cfg_deleteItem(char * name){
    CFG_ITEM * item;

    item=cfg_getItem(name);

    if(item!=NULL){

        item->deleted=true;

        return true;
    }

    return false;
}

void cfg_printItems(){
    int i;

    for(i=0;i<cfg_itemCount;++i){
        if (cfg_items[i].deleted){
            printk("i=%d deleted!\n",i);
        }else{
            if (cfg_items[i].dummy){
                printk("i=%d dummy! text='%s'\n",i,cfg_items[i].name);
            }else{
                printk("i=%d name='%s' value='%s'\n",i,cfg_items[i].name,cfg_items[i].value);
            }
        }
    }
}
