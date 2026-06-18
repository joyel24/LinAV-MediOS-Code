/*************************************
  Little John GP32
  File : unes_db.c
  Authors : Yoyo.
  Version : 0.2
  Last update : 7th July 2003
**************************************/


#ifdef __GP32__
#include "gpdef.h"
#include "gpstdlib.h"
#include "gpgraphic.h"
#include "gpfont.h"
#include "gpmm.h"
#include "gpstdio.h"



#include "unes_db.h"

#include "unes_str_func.h"

#include "unzip.h"


nestoy_info *db_info;
uint32 num_dbentries;

int32 fagets(char *buf,uint32 smax,F_HANDLE fp)
{
	int32 i;
	long l,dummy;
	long readsize;
				
	gm_memset(buf,0,256);
	GpFileRead(fp, buf, 256, (ulong*)&readsize);
	if (buf[0]==0) return 0;
		
	i=0;
	while (i<readsize)
	{
		if (buf[i]==0x0D) buf[i]=0;
		if (buf[i]==0x0A) break;
		i++;
	}
   	GpFileSeek(fp, FROM_CURRENT, -readsize+i+1, NULL);
	
	return 1;
}


uint32 atoiHex(char *c)
{
	uint32 i,res,j;	
	i=res=0;
	while (c[i])
	{		
		res=res*16;		
		if ( (c[i]>='0')&&(c[i]<='9') ) res+=c[i]-'0';
		if ( (c[i]>='A')&&(c[i]<='F') ) res+=c[i]-'A'+10;

		if ( (c[i]>='a')&&(c[i]<='f') ) res+=c[i]-'a'+10;		
		i++;
	}
	return res;
}

uint8 init_memdb()
{
  F_HANDLE f;
  ulong fsize;
  uint8 *buf;
  uint32 num_entries;
  uint32 i,j,a,b;
  char tmp[256];
  int err_code;
  ulong readsize;
  
  db_info=NULL;
  num_dbentries=0;
	
  
  err_code = GpFileGetSize("gp:\\gpmm\\nes\\nesdbase.dat", &fsize);
  if (err_code == SM_OK)
  {
  	buf=(uint8*)gm_malloc(fsize);
  	if (!buf)
  	{  
  	  return 2;
  	}
  	err_code = GpFileOpen("gp:\\gpmm\\nes\\nesdbase.dat", OPEN_R, &f);
  	if (err_code != SM_OK) return 2;  	
    err_code = GpFileRead(f, buf, fsize, &readsize);
  	GpFileClose(f);  	
  	if (err_code != SM_OK) return 2;
  	  	
  	i=0;
  	num_entries=0;
  	while (i<fsize)
  	{
  		if (buf[i]==0x0A) num_entries++;
  		i++;
  	}
  	db_info=(nestoy_info*)gm_malloc(sizeof(nestoy_info)*num_entries);
  	if (!db_info)
  	{
  		gm_free(buf);
  		return 3;
  	}
 	i=0;
 	a=0; //depart
 	b=0; //fin
 	while (i<num_entries)
 	{ 		
 		gm_memset(&db_info[i],0,sizeof(nestoy_info));
 		b=a; 		
 		while (buf[b]!=0x0D) b++;
 		tmp[8]=0;
 		gm_memcpy(tmp,buf+a,8); 		
 		db_info[i].crcall=atoiHex(tmp);
 		
 		//a cause de l' * qui apparait parfosi pares le crc et avant le ';'
 		if (buf[a+9]==';') a++; 		
 		gm_memcpy(tmp,buf+a+9,8); 		
 		db_info[i].crc=atoiHex(tmp);
 		
 		//a cause de l' * qui apparait parfosi pares le crc et avant le ';'
 		if (buf[a+18]==';') a++;
 		j=a+18; 		
 		while ((buf[j]!=';')&&(j<b)) j++;
  		if (j==b) 
  		{
  			gm_free(buf);
  			gm_free(db_info);
  			return 4;	
  		}
 		gm_memcpy(db_info[i].title,buf+a+18,j-a-18); 		
 		db_info[i].title[j-a-18]=0;
 		
 		//header1
 		j++;
 		a=j; 		
 		while ((buf[j]!=';')&&(j<b)) j++;
  		if (j==b) 
  		{
  			gm_free(buf);
  			gm_free(db_info);
  			return 4;	
  		}
  		gm_memcpy(tmp,buf+a,j-a);
  		tmp[j-a]=0;
 		db_info[i].header1=yo_atoi(tmp);
 		
 		//header2
 		j++;
 		a=j; 		
 		while ((buf[j]!=';')&&(j<b)) j++;
  		if (j==b) 
  		{
  			gm_free(buf);
  			gm_free(db_info);
  			return 4;	
  		}
  		gm_memcpy(tmp,buf+a,j-a);
  		tmp[j-a]=0;
 		db_info[i].header2=yo_atoi(tmp);
 		
 		//rom_prg_nb
 		j++;
 		a=j; 		
 		while ((buf[j]!=';')&&(j<b)) j++;
  		if (j==b) 
  		{
  			gm_free(buf);
  			gm_free(db_info);
  			return 4;	
  		}
  		gm_memcpy(tmp,buf+a,j-a);
  		tmp[j-a]=0;
 		db_info[i].prg_rom_nb=yo_atoi(tmp);
 		
 		//chr_prg_nb
 		j++;
 		a=j; 		
 		while ((buf[j]!=';')&&(j<b)) j++;
  		if (j==b) 
  		{
  			gm_free(buf);
  			gm_free(db_info);
  			return 4;	
  		}
  		gm_memcpy(tmp,buf+a,j-a);
  		tmp[j-a]=0;
 		db_info[i].chr_rom_nb=yo_atoi(tmp);
 		
 		
 		//country
 		j++;
 		a=j;
 		while ((buf[j]!=';')&&(j<b)) j++;  		
  		gm_memcpy(db_info[i].country,buf+a,j-a);
  		db_info[i].country[j-a]=0;  		
 		
 		a=b+2;
 		i++;
 		
 	}
 	num_dbentries=num_entries;
 	gm_free(buf);
 	return 0;
  }
  else return 1;  
}

void free_memdb()
{
	if (db_info) gm_free(db_info);
}



uint8 db_getinfo(uint32 crc,uint32 crcall,nestoy_info *ninfo)
{
  F_HANDLE f;
  char realcrc[10],dbinfo[256],tmp[10];  
  uint8 found,len;
  uint32 i,j;
  	
  gm_sprintf(realcrc,"%08X",crc);

  if (GpFileOpen("gp:\\gpmm\\nes\\nesdbase.dat", OPEN_R, &f) == SM_OK)
  {  	
  	found=0;
  	/* on recupere la ligne*/  	
  	while ((fagets(dbinfo, 256, f))&&(!found))  	
  	{
  		/*on recupere le crc prom*/
  		char crcstr[10];
  		i=0;
  		while (dbinfo[i]!=';') i++;
  		i++;
  		gm_memcpy(crcstr,dbinfo+i,8);
  		crcstr[8]=0;
  		/*conversion maj si nécessaire*/
  		//for (i=0;i<8;i++) if (crcstr[i]>='a' && crcstr[i]<='z') crcstr[i]+='A'-'a';
  		yo_strupr(crcstr);
  		/*si identique, on a trouvé un candidat*/
  		if (gm_compare(realcrc,crcstr)==0) 
  		{
  			found=1;
  			break;
  		}
  	}
  	GpFileClose(f);
  	if (found) 
  	{  		
  		gm_memset(ninfo,0,sizeof(nestoy_info));
  		
  		len=0;
  		while (dbinfo[len]) len++;
  		//CRCALL
  		i=0;
  		while (dbinfo[i]!=';') i++;  		
  		//CRC  		
  		i++;
  		while (dbinfo[i]!=';') i++;  		
  		//name
  		i++;
  		j=i;
  		while ((dbinfo[i]!=';')&&(i<len)) i++;
  		if (i==len) return 3;
  		gm_memcpy(ninfo->title,dbinfo+j,i-j);
  		ninfo->title[i-j]=0;  		  		  		
  		  		
  		//header1
  		i++;
  		j=i;
  		while ((dbinfo[i]!=';')&&(i<len)) i++;
  		if (i==len) return 3;
  		gm_memcpy(tmp,dbinfo+j,i-j);
  		tmp[i-j]=0;
  		ninfo->header1=yo_atoi(tmp);
  		
  		//header2
  		i++;
  		j=i;
  		while ((dbinfo[i]!=';')&&(i<len)) i++;
  		if (i==len) return 3;
  		gm_memcpy(tmp,dbinfo+j,i-j);
  		tmp[i-j]=0;
  		ninfo->header2=yo_atoi(tmp);
  		
  		//prg_rom_nb
  		i++;
  		j=i;
  		while ((dbinfo[i]!=';')&&(i<len)) i++;
  		if (i==len) return 3;
  		gm_memcpy(tmp,dbinfo+j,i-j);
  		tmp[i-j]=0;
  		ninfo->prg_rom_nb=yo_atoi(tmp);
  		
  		//chr_rom_nb
  		i++;
  		j=i;
  		while ((dbinfo[i]!=';')&&(i<len)) i++;
  		if (i==len) return 3;
  		gm_memcpy(tmp,dbinfo+j,i-j);
  		tmp[i-j]=0;
  		ninfo->chr_rom_nb=yo_atoi(tmp);
  		
  		//country
  		i++;
  		j=i;
  		while ((dbinfo[i]!=';')&&(i<len)) i++;
  		if (i==j) return 3;  		
  		gm_memcpy(ninfo->country,dbinfo+j,i-j);
  		ninfo->country[i-j]=0;
  		if (i==len) return 0;
  		  		
  		
  		//optional fields
  		 //publisher
  		 i++;
  		 j=i;
  		 while ((dbinfo[i]!=';')&&(i<len)) i++;
  		 if (j-i)
  		 {
  			 gm_memcpy(ninfo->publisher,dbinfo+j,i-j);
  			 ninfo->publisher[i-j]=0;
  		 }
  		 else return 0;
  		
  		 //date
  		 i++;
  		 j=i;
  		 while ((dbinfo[i]!=';')&&(i<len)) i++;
  		 if (j-i)
  		 {
  			 gm_memcpy(ninfo->date,dbinfo+j,i-j);
  			 ninfo->date[i-j]=0;
  		 }
  		 return 0;  	
  	}
  	else return 2;
  }
  else return 1;
}


uint8 dbmem_getinfo(uint32 crc,uint32 crcall,nestoy_info *ninfo)
{
  uint32 i;      
  if (!db_info) return 1;
  if (!num_dbentries) return 1;   
     
     
  for (i=0;i<num_dbentries;i++) 
    if (crc==db_info[i].crc)
    {
    	gm_memcpy(ninfo,&db_info[i],sizeof(nestoy_info));
    	return 0;
    }
 return 2;
}

#endif
