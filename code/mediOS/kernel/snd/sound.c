/*
*   kernel/driver/sound.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/stdfs.h>

#include <kernel/mas.h>
#include <kernel/aic23.h>

#include <kernel/sound.h>
#include <kernel/mp3_data.h>
#include <sys_def/sound.h>

#define SND_BUFF_SIZE 3*1024*1024

sound_buffer_s * buff;
int file_size;
int fd=-1;
int v1first = 0;
int endOfList;

void sound_readMore(void)
{
    if(fd<0) /* this should only happen for the first file*/
    {
        if(!openFileFromList(curEntryInList()))
        {
            endOfList=1;
            return;
        }
    }

    
    if(file_size == ftell(fd))
        if(!openFileFromList(nxtEntryInList()))
        {
            endOfList=1;
            return;
        }
        
    sound_fillBuffer();
}

void sound_fillBuffer(void)
{
    int nbRead,free_space,size,leftInFile,toRead;
    
    leftInFile=file_size-ftell(fd);
    
    free_space=buff->read-buff->write;
    if(free_space<=0)
        free_space+=buff->size;
    
    if(leftInFile>free_space)
    {
        if(free_space>(SND_BUFF_SIZE>>1))
            size=SND_BUFF_SIZE>>1;
        else
            return;
    }
    else
    {
        size = leftInFile;
    }

    printk("need to read: %x\n",size);

    if(buff->write>=buff->read)
    {
        toRead=min(size,buff->size-buff->write);
        nbRead=read(fd,buff->data+buff->write,toRead);
        buff->write+=nbRead;
        size-=nbRead;
        if(buff->write>=buff->size)
        {
            buff->write=0;
            if(size>0)
            {
                buff->write+=read(fd,buff->data+buff->write,size);
                if(buff->write>=buff->size)
                    buff->write=0;
            }
        }
    }
    else
    {
        toRead=min(size,buff->read-buff->write);
        buff->write+=read(fd,buff->data+buff->write,toRead);
        if(buff->write>=buff->size)
            buff->write=0;
    }

    printk("Buffer state: read=%x, write=%x\n",buff->read,buff->write);
    
    if(!buff->playing)
        buff->startPlayback();
}

int openFileFromList(struct list_entry * cur_entry)
{
    if(!cur_entry)
    {
        //printk("no file in list\n");
        return 0;
    }

    if(fd>=0)
        close(fd);
    fd=open(cur_entry->id3.path,O_RDONLY);
    if(fd<0)
    {
        printk("Can't open file %s\n",cur_entry->id3.path);
        return 0;
    }

    file_size = filesize(fd);

    //fprintf(stderr,"%s (s=%d) opened\n",cur_entry->id3.path,file_size);

    lseek(fd,cur_entry->id3.first_frame_offset,SEEK_SET);

    debug_info(cur_entry->id3.path,&cur_entry->id3);

    return 1;
}

void sound_initMp3(void)
{
    sound_initPlayer();
    
#if defined(HAVE_MAS_SOUND)
    mas_IniMp3(buff);
#elif defined(HAVE_AIC23_SOUND)
    #warning need mp3 ini for AIC
#else
#warning No Sound chio defined
#endif
}

void sound_freeMp3(void)
{
    if(fd>=0)
        close(fd);
    cleanPlaylist();
}

void sound_initPlayer(void)
{
    if(buff==NULL)
    {
        buff = (sound_buffer_s *)malloc(sizeof(sound_buffer_s));

        buff->data = (char*)malloc(sizeof(char)*SND_BUFF_SIZE);
        buff->size=SND_BUFF_SIZE;

        buff->read=buff->write=buff->playing=0;
        buff->startPlayback=buff->pausePlayback=NULL;
    }
}

void sound_startMp3(void)
{
    printk("calling chip mp3 start\n");
    if(buff->startPlayback)
        buff->startPlayback();
}

void sound_pauseMp3(void)
{
    printk("calling chip mp3 pause\n");
    if(buff->pausePlayback)
        buff->pausePlayback();
}

void sound_nxtTrack(void)
{
    printk("nxtTck\n");
    if(!nxtEntryInList())
    {
        printk("EO list nothing to do\n");
        return;
    }
    
    if(!openFileFromList(curEntryInList()))
    {
        printk("Can't open file\n");
        return;
    }
    sound_pauseMp3();
    buff->write = buff->read = 0;
    sound_fillBuffer();
}

void sound_prevTrack(void)
{
    if(!prevEntryInList())
    {
        printk("Head of list => reload snd file\n");
    }
    
    if(!openFileFromList(curEntryInList()))
    {
        printk("Can't open file\n");
        return;
    }
    sound_pauseMp3();
    buff->write = buff->read = 0;
    sound_fillBuffer();
}



/********************* MIXER                 ***************************/

void sound_mixerCtl(unsigned int cmd, int dir, void * arg)
{
    /*int * val=(int*)arg;
    int tmp;


    switch(cmd)
    {
        case MIXER_VOLUME:
            if(dir==MAS_SET)
            {
                mas_control_config(MAS_SET,MAS_VOLUME,*val);
                oldVol=*val;
            }
            else
            {
                *val=mas_control_config(MAS_GET,MAS_VOLUME,*val);
            }
            break;
        case MIXER_BALANCE:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_BALANCE,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_BALANCE,*val);
            break;
        case MIXER_MUTE:
            if(dir==MAS_SET)
            {
                if(*val)
                    mas_control_config(MAS_SET,MAS_VOLUME,0);
                else
                    mas_control_config(MAS_SET,MAS_VOLUME,oldVol);
            }
            else
            {
                tmp=mas_control_config(MAS_GET,MAS_VOLUME,*val);
                if(tmp==0)
                    *val=1;
                else
                    *val=0;
            }
            break;
        case MIXER_BASS:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_BASS,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_BASS,*val);
            break;
        case MIXER_TREBLE:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_TREBLE,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_TREBLE,*val);
            break;
        case MIXER_LOUDNESS:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_LOUDNESS,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_LOUDNESS,*val);
            break;
        case MIXER_MIC_GAIN:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_MICRO_GAIN,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_MICRO_GAIN,*val);
            break;
        case MIXER_ADC_L_GAIN:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_ADC_L_GAIN,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_ADC_L_GAIN,*val);
            break;
        case MIXER_ADC_R_GAIN:
            if(dir==MAS_SET)
                mas_control_config(MAS_SET,MAS_ADC_R_GAIN,*val);
            else
                *val=mas_control_config(MAS_GET,MAS_ADC_R_GAIN,*val);
            break;
        default:
                printk("[ctl MIX] bad ctl\n");
    }*/

}


/********************* sound init               ***************************/

void sound_init(void)
{

#ifdef HAVE_MAS_SOUND
    mas_init();
#endif

#ifdef HAVE_AIC23_SOUND
    aic23_init();
#endif
   iniPlaylist();
   endOfList=0;
   fd = -1;
   buff=NULL;
   printk("[init] sound done\n");
}
