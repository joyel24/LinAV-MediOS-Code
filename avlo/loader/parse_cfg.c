#include "parse_cfg.h"
#include "string.h"
#include <file.h>
#include <debug.h>

int file;

int line_num;
int back = 0; /* can go back by one char */

char item_buff[MAX_TOKEN+1];
char value_buff[MAX_TOKEN+1];

int file_open(char *name)
{
    if ((file = fopen(name,"r"))<0)
    {
    	debug("error reading config file %s\n",name);
	return -1;
    }
    line_num = 1;
    return 0;
}

void file_close()
{
	fclose(file);
}

int next_char(void)
{
    int ch;

    if (!back) return fgetc(file);
    ch = back;
    back = 0;
    return ch;
}

void again(int ch)
{
    back = ch;
}

int nxt_token(char * buff)
{
    char *here;
    int ch;

/* processing empty lines and comments */
    while (1) {
	while ((ch = next_char()), ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
	    if (ch == '\n' || ch == '\r') line_num++;
	if (feof(file)) return 0;
	if (ch != '#') break;
	while ((ch = next_char()), (ch != '\n' && ch != '\r'))
	    if (feof(file)) return 0;
	line_num++;
    }
/* processing '=' char */    
    if (ch == '=')
    {
    	buff[0]='=';
	buff[1]=0;
    	return 1;
    }
/* processing quoted string */
    if (ch == '"') {
	here = buff;
	while (here-buff < MAX_TOKEN) {
	    ch = next_char();
	    if (feof(file)) debug("EOF in quoted string");
	    if (ch == '"') {
		*here = 0;
		return 1;
	    }
	    if (ch == '\n' || ch == '\r' || ch == '\t')
		debug("\\n and \\t are not allowed in quoted strings");
	    *here++ = ch;
	}
	debug("Quoted string is too long");
	return 0; /* not reached */
    }
/* processing normal char */
    here = buff;
    while (here-buff < MAX_TOKEN) {
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '#' ||
	      ch == '=' || feof(file)) {
		again(ch);
		*here = 0;
		return 1;
	    }
	*here++ = ch;
	ch = next_char();
    }
    debug("Token is too long");
    return 0; /* not reached */
}

int nxt_cfg(char *item,char *value)
{    
    if (!nxt_token(item)) return 0;
    
    debug("[nxt_cfg]: item=|%s|\n",item);
    
    if (!strcmp(item,"="))
    {
    	debug("Syntax error");
	return 0;
    }
    
    if (!nxt_token(value)) return 0;
    if (strcmp(value,"="))
    {
        debug("Error '=' expected (get: %s - l:%d)\n",value,line_num);
        return 0;
    }
    if (!nxt_token(value))
    {
    	debug("Value expected");
	return 0;
    }
    
    debug("[nxt_cfg]: value=|%s|\n",value);
    
    if (!strcmp(value,"="))
    {
    	debug("Syntax error after %s",item);
	return 0;
    }
    return 1;
}

void iniCfg(struct config_image * cfg)
{
	cfg->image[0]=0;
	cfg->label[0]=0;
	cfg->append[0]=0;
	cfg->comment[0]=0;
}

void iniCfgG(struct config_gene * cfgG)
{
	cfgG->defBin[0]=0;
        cfgG->key[0]=0;
        cfgG->repeat=0;
        cfgG->timeOut=0;
}

int chkCfg(struct config_image * cfg)
{
	if(cfg->image[0] != 0 && cfg->label[0] !=0)
		return 1;
	else
	{
		debug("invalide config %s %s\n",cfg->image,cfg->label);
		return 0;
	}
}	

int do_parse(struct config_image * cfg,struct config_gene * cfgG)
{
    char *item=item_buff;
    char *value=value_buff;
    int cfg_id=-1;
    int pos;
    
    for(pos=0;pos<MAX_CFG;pos++)
		iniCfg(&cfg[pos]);
    	iniCfgG(cfgG);
	
    while (1) {
	if (!nxt_cfg(item,value))
        {
        	//cfg_id=-1;
        	break;
        }
        
	if(!strcmp(item,"image"))
	{
		if(cfg_id>=0)
		{
			if(chkCfg(&cfg[cfg_id]))
			{
				cfg_id++;
				if(cfg_id>=MAX_CFG)
				{
					debug("too much images defined\n");
					//return cfg_id-1;
				}
			}
			else
				iniCfg(&(cfg[cfg_id]));
		}
		else
		{
			cfg_id++;		
			if(cfg_id>=MAX_CFG)
			{
				debug("too much images defined\n");
				//return cfg_id-1;
			}
		}
		strcpy(cfg[cfg_id].image,value);
	}
	else if(!strcmp(item,"label"))
	{
		if(cfg_id<0)
		{
			debug("label param before image\n");
		}
		else
		{
			strcpy(cfg[cfg_id].label,value);
		}
		
	}
	else if(!strcmp(item,"append"))
	{
		if(cfg_id<0)
		{
			debug("append param before image\n");
		}
		else
		{
			strcpy(cfg[cfg_id].append,value);
		}
	}
	else if(!strcmp(item,"default"))
	{
		strcpy(cfgG->defBin,value);
	}
        else if(!strcmp(item,"key"))
	{
		strcpy(cfgG->key,value);
	}
        else if(!strcmp(item,"repeat"))
	{
		cfgG->repeat=atoi(value);
	}
        else if(!strcmp(item,"time-out"))
	{
		cfgG->timeOut=atoi(value);
	}
	else
		debug("unknown item type: %s on line %d (cfg_id=%d)\n",item,line_num,cfg_id);
    }
    return cfg_id;    
}



