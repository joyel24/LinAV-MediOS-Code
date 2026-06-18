/*
*   include/kernel/mp3_data.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

/***************************************************************************
 *
 * Parts of this code is comming from the Rockbox project.
 *
 * Copyright (C) 2002 by Linus Nielsen Feltzing
 *
  ****************************************************************************/

#ifndef _MP3DATA_H_
#define _MP3DATA_H_

#define MPEG_VERSION2_5 0
#define MPEG_VERSION1   1
#define MPEG_VERSION2   2

#include <kernel/id3.h>

struct mp3info {
    /* Standard MP3 frame header fields */
    int version;
    int layer;
    int protection;
    int bitrate;
    int frequency;
    int padding;
    int channel_mode;
    int mode_extension;
    int emphasis;
    int frame_size; /* Frame size in bytes */
    int frame_time; /* Frame duration in milliseconds */

    int is_vbr;     /* 1 if the file is VBR */
    int has_toc;    /* 1 if there is a VBR header in the file */
    int is_xing_vbr; /* 1 if the VBR header is of Xing type */
    int is_vbri_vbr; /* 1 if the VBR header is of VBRI type */
    unsigned char toc[100];
    int frame_count; /* Number of frames in the file (if VBR) */
    int byte_count;  /* File size in bytes */
    int file_time;   /* Length of the whole file in milliseconds */
    int vbr_header_pos;
};

/* Xing header information */
#define VBR_FRAMES_FLAG 0x01
#define VBR_BYTES_FLAG  0x02
#define VBR_TOC_FLAG    0x04


unsigned long find_next_frame(int fd, int *offset, int max_offset, unsigned long last_header);
unsigned long mem_find_next_frame(int startpos, int *offset, int max_offset,
                                  unsigned long last_header);
int get_mp3file_info(int fd, struct mp3info *info);
int count_mp3_frames(int fd, int startpos, int filesize,
                     void (*progressfunc)(int));
int create_xing_header(int fd, int startpos, int filesize,
                       unsigned char *buf, int num_frames,
                       unsigned long header_template,
                       void (*progressfunc)(int), int generate_toc);
void debug_info(char * filename,struct mp3entry * mp3);
#endif
