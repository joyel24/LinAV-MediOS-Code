/*
*   include/kernel/id3.h
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
 * Copyright (C) 2002 by Daniel Stenberg
 *
 ****************************************************************************/
#ifndef ID3_H
#define ID3_H

struct mp3entry {
    char path[255];
    char *title;
    char *artist;
    char *album;
    char* genre_string ;
    char* track_string ;
    char* year_string ;
    char* composer ;
    int tracknum;
    int version;
    int layer;
    int year;
    unsigned char id3version;
    unsigned char genre;
    unsigned int bitrate;
    unsigned int frequency;
    unsigned int id3v2len;
    unsigned int id3v1len;
    unsigned int first_frame_offset; /* Byte offset to first real MP3 frame.
                                        Used for skipping leading garbage to
                                        avoid gaps between tracks. */
    unsigned int vbr_header_pos;
    unsigned int filesize; /* in bytes */
    unsigned int length;   /* song length */
    unsigned int elapsed;  /* ms played */

    /* MP3 stream specific info */
    long bpf;              /* bytes per frame */
    long tpf;              /* time per frame */

    /* Xing VBR fields */
    int vbr;
    int has_toc;    /* 1 if there is a VBR header in the file*/
    unsigned char toc[100];/* table of contents */

    /* these following two fields are used for local buffering */
    char id3v2buf[300];
    char id3v1buf[3][32];

    /* resume related */
    int offset;            /* bytes played */
    int index;             /* playlist index */
};

enum {
    ID3_VER_1_0 = 1,
    ID3_VER_1_1,
    ID3_VER_2_2,
    ID3_VER_2_3,
    ID3_VER_2_4
};

int mp3info(struct mp3entry *entry, char *filename, int v1first);
char* id3_get_genre(const struct mp3entry* id3);

#endif
