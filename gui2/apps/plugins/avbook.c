/*
* avbook.c
* by Schoki 2004
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include "stdlib.h"
#include "stdio.h"

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"
#include "font.h"

#define true 1
#define false 0

#define MAX_LABEL 20
#define LABELFIELD_OFFSET 2

enum
{
    FieldString  = 0,
    FieldNumber  = 1,
    FieldDate    = 2,
    FieldPicture = 3
};

typedef struct
{
  char strLabel[MAX_LABEL];
  int  type;
  int  xPos;
  int  yPos;
  int  width;
  int  height;
  int  maxChars;
} EntryInfoT;

struct client_operations * cops;
EntryInfoT* pEntryInfo;  // Felderbeschreibungen
struct AddressDataT* pAvData;   // Datensaetze mit Inhalt
int *pListColumns;

int g_fields = 0;
int g_ListColumns = 0;
int g_ShowFieldNames = true;
int g_StartWithListView = true;

typedef struct
{
    char* pData;   // so gro? wie Anzahl EntryInfoT, d.h. Anzahl Eingabefelder
} AddressDataT;

int eventHandler(int evt)
{
    switch (evt) {

        case BTN_F1:

            if(g_StartWithListView == true)
                g_StartWithListView = false;
            else
                g_StartWithListView = true;

            InitAvBook();
            
            break;

        case BTN_OFF:
        case EVT_QUIT:
            /* get out of here */
            RELEASE(cops)
            break;
    }
    return 1;
}

int ReadConfig()
{
    char item_buff[MAX_TOKEN+1];
    char value_buff[MAX_TOKEN+1];
    char *item=item_buff;
    char *value=value_buff;
    char tmp[MAX_TOKEN];

    int field = 0;
    int listcolumns = 0;

    cops->openCfg("/mnt/avwm/plugins/avbook.cfg",CFG_READ);

    // Calculate size of field array
    while (1)
    {
        /* get next item/value couple, returns 0 if ther is no more item/value in the file */
        if (!cops->getCfg(item,value)) break;
        /* now parse the item using strcmp */

        sprintf(tmp,"Label%d",g_fields);
        if(!strcmp(item,tmp))
        {
            g_fields++;
        }

        sprintf(tmp,"Column%d",g_ListColumns);
        if(!strcmp(item,tmp))
        {
            g_ListColumns++;
        }
    }

    cops->closeCfg();

    // Get Memory for fields
    if(g_fields > 0)
        pEntryInfo = (EntryInfoT *)malloc(sizeof(EntryInfoT)*g_fields);

    // Get Memory for Listcolumns
    if(g_ListColumns > 0)
        pListColumns = (int *)malloc(sizeof(int)*g_ListColumns);

    cops->openCfg("/mnt/avwm/plugins/avbook.cfg",CFG_READ);

    while (1)
    {
        // get next item/value couple, returns 0 if ther is no more item/value in the file
        if (!cops->getCfg(item,value)) break;
        // now parse the item using strcmp

        /////////////////////////////////////
        // Read Field descriptions
        /////////////////////////////////////

        sprintf(tmp,"Label%d",field);
        if(!strcmp(item,tmp))
        {
            strcpy(pEntryInfo[field].strLabel,value);
        }

        sprintf(tmp,"Type%d",field);
        if(!strcmp(item,tmp))
        {
            pEntryInfo[field].type=atoi(value);
        }

        sprintf(tmp,"XPos%d",field);
        if(!strcmp(item,tmp))
        {
            pEntryInfo[field].xPos=atoi(value);
        }

        sprintf(tmp,"YPos%d",field);
        if(!strcmp(item,tmp))
        {
            pEntryInfo[field].yPos=atoi(value);
        }

        sprintf(tmp,"Width%d",field);
        if(!strcmp(item,tmp))
        {
            pEntryInfo[field].width=atoi(value);
        }

        sprintf(tmp,"Height%d",field);
        if(!strcmp(item,tmp))
        {
            pEntryInfo[field].height=atoi(value);
        }

        sprintf(tmp,"MaxChars%d",field);
        if(!strcmp(item,tmp))
        {
            pEntryInfo[field].maxChars=atoi(value);
            field++;
        }

        /////////////////////////////////////
        // Read Listview configuration
        /////////////////////////////////////
        sprintf(tmp,"Column%d",listcolumns);
        if(!strcmp(item,tmp))
        {
            pListColumns[listcolumns]=atoi(value);
            listcolumns++;
        }

        /////////////////////////////////////
        // Read Generell Settings
        /////////////////////////////////////
        if(!strcmp(item,"ShowFieldNames"))
        {
            g_ShowFieldNames=atoi(value);
            if( (g_ShowFieldNames < 0) || (g_ShowFieldNames > 1) )
                g_ShowFieldNames = true;
        }

        if(!strcmp(item,"StartWithListView=1"))
        {
            g_StartWithListView=atoi(value);
            if( (g_StartWithListView < 0) || (g_StartWithListView > 1) )
                g_StartWithListView = true;
        }
    }

    cops->closeCfg();

    return 1;
}

int SetFontSize(int height)
{
    int fontheight = 0;

    if(height < 6)
    {
        cops->setFont(STD4X6);
        fontheight = 6;
    }
    else if(height < 8)
    {
        cops->setFont(STD4X8);
        fontheight = 8;
    }
    else if(height < 10)
    {
        cops->setFont(STD6X10);
        fontheight = 10;
    }
    else if(height < 12)
    {
        cops->setFont(STD6X12);
        fontheight = 12;
    }
    else if(height < 13)
    {
        cops->setFont(STD6X13);
        fontheight = 13;
    }
    else
    {
        cops->setFont(STD7X14);
        fontheight = 14;
    }

    return fontheight;
}

void ShowEditView()
{
    int fontheight = 0;
    int w = 0;
    int h = 0;
    int i = 0;

    for(i = 0; i < g_fields;i++)
    {
        fontheight = SetFontSize(pEntryInfo[i].height);

        if(pEntryInfo[i].type != FieldPicture)
            pEntryInfo[i].height = fontheight;

        if(g_ShowFieldNames == true)
        {
            cops->getStringS(pEntryInfo[i].strLabel, &w,&h);
            cops->putS(COLOR_BLACK,
                       COLOR_LIGHT_BLUE,
                       pEntryInfo[i].xPos,
                       pEntryInfo[i].yPos,
                        pEntryInfo[i].strLabel);
            cops->fillRect( COLOR_LIGHT_GREY,
                            pEntryInfo[i].xPos+w+LABELFIELD_OFFSET,
                            pEntryInfo[i].yPos,
                            pEntryInfo[i].width,
                            pEntryInfo[i].height);
        }
        else
        {
            cops->fillRect( COLOR_LIGHT_GREY,
                            pEntryInfo[i].xPos,
                            pEntryInfo[i].yPos,
                            pEntryInfo[i].width,
                            pEntryInfo[i].height);
        }
    }
}

void ShowListView()
{
    int fontheight = 0;
    int w = 0;
    int h = 0;
    int i = 0;
    int xPos = 0;

    cops->setFont(STD6X9);

    if(g_ShowFieldNames == true)
    {
        for(i = 0; i < g_ListColumns;i++)
        {
            cops->fillRect( COLOR_LIGHT_GREY,
                            xPos,
                            15,
                            pEntryInfo[pListColumns[i]].width,
                            9);
            cops->putS( COLOR_BLACK,
                        COLOR_LIGHT_GREY,
                        xPos,
                        15,
                        pEntryInfo[pListColumns[i]].strLabel);

            xPos += pEntryInfo[pListColumns[i]].width;
        }
    }
}

void ShowHeader()
{
    cops->setFont(STD6X9);

    cops->fillRect(COLOR_BLUE,0,0,320,13);
    cops->putS(COLOR_GREY, COLOR_BLUE, 5,2, "AvBook V0.1 by SCHOKI");
    cops->fillRect(COLOR_BLACK,0,14,320,2);
    cops->fillRect(COLOR_LIGHT_BLUE,0,15,320,225);
}

void InitAvBook()
{
    cops->clearScreen(COLOR_WHITE);
    cops->setFont(STD6X9);

    ShowHeader();

    if(g_StartWithListView == true)
        ShowListView();
    else
        ShowEditView();
}

void QuitAvBook()
{
    if(pEntryInfo != 0)
        free(pEntryInfo);

    if(pListColumns != 0)
        free(pListColumns);
}

/* called function from outside */
int main(int argc,char * * argv)
{
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();

    ReadConfig();

    InitAvBook();

    PACK(cops,NULL)

    QuitAvBook();

    return 0;
}

