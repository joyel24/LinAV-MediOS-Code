/*
* parse_cfg.h
*
* avos - http://avos.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __PARSE_CFG_H
#define __PARSE_CFG_H

#define MAX_TOKEN 60
#define MAX_CFG 10

struct config_image {
	char label[MAX_TOKEN];
	char image[MAX_TOKEN];
	char comment[MAX_TOKEN];
	char append[MAX_TOKEN];	
};

struct config_gene {
	char defBin[MAX_TOKEN];
       // char key[MAX_TOKEN];
        int repeat;
        int timeOut;
};

int file_open(char *name);
void file_close();
int next_char(void);
void again(int ch);
int nxt_token(char * buff);
int nxt_cfg(char *item,char *value);
int do_parse(struct config_image * cfg,struct config_gene * cfgG);
void iniCfg(struct config_image * cfg);
void iniCfgG(struct config_gene * cfgG);
int chkCfg(struct config_image * cfg);


#endif
