/*
   abook.c, a simple adressbook for AV3xx

   Copyright 2004, Goetz Minuth

   This File is free software; I give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.
*/

#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <rtc.h>
#include <ata.h>
#include <file.h>
#include <string.h>

//make structures for each video component/plane we'll be using
struct graphicsBuffer screenBitmap;
struct graphicsBuffer screenBitmap2;
struct graphicsBuffer screenVideo;
struct graphicsBuffer spriteFont6x10;

//make two-color pallettes for text on video (pal32) and bitmap (pal16) planes.
//First number is background color, second is text color.
static int pal32[2] = {0x00800080, 0xff80ff80}; //Y2CrY1Cb order, 32-bit color: {black,white}
static int pal16[2] = {0x0101, 0xffff}; //indexes to the 256-color pallette: {red,white}

static struct tm* ourTime;

/*********************** GLOBAL VARIABLES **********************************/

/////////////////////////////////////////////////
// COLORS
int ADDRESS_TEXTCOLOR_HEAD   = 0xFFFF;
int ADDRESS_TEXTCOLOR_LIST   = 0xFBFB;
int ADDRESS_TEXTCOLOR_STATUS = 0xFBFB;
int ADDRESS_TEXTCOLOR_MENU   = 0xFBFB;
int ADDRESS_TEXTCOLOR_ELEMENTNAME  = 0xFBFB;
int ADDRESS_TEXTCOLOR_ELEMENTENTRY = 0x0000;

int ADDRESS_BACKCOLOR_HEAD         =  0xF9F9;
int ADDRESS_BACKCOLOR_LIST         =  0xB9B9;
int ADDRESS_BACKCOLOR_STATUS       =  0xC1C1;
int ADDRESS_BACKCOLOR_MENU         =  0xC9C9;
int ADDRESS_BACKCOLOR_SEPERATOR    =  0x0000; // unter Kopfzeile
int ADDRESS_BACKCOLOR_ELEMENTNAME  =  0xB9B9;
int ADDRESS_BACKCOLOR_ELEMENTENTRY =  0xFFFF;
int ADDRESS_BACKCOLOR_MARKER       =  0xF9F9;
int ADDRESS_BACKCOLOR_UNDERLINER   =  0x0000;

int ADDRESS_TEXTCOLOR_MESSAGE        = 0x00800080; // black
int ADDRESS_BACKCOLOR_MESSAGE        = 0xFF44d915; //
int ADDRESS_BACKCOLOR_MESSAGE_BORDER = 0x50FF5050; //
int ADDRESS_BACKCOLOR_MESSAGE_SEP    = 0x00800080; // black

/////////////////////////////////////////////////

#define MAX_DATA 1000        // max. Anzahl von Datensaetzen
#define MAX_ELEMENTS 10
#define MAX_ENTRIES_PAGE 15  // max. Anzahl von Datensaetzen pro Seite

#define MAX_INI_ROW_LENGTH 150 // max. Anzahl Zeichen einer Zeile in der Init-Datei

#define ASCII_SET_BEGIN 32
#define ASCII_SET_END 126
#define ASCII_DEFAULT 65

#define RET_OK     1
#define RET_CANCEL 0

#define MAX_ELEMENT_CHARS 41
#define MAX_NAME 41
#define MAX_FIRSTNAME 21
#define MAX_STREET 41
#define MAX_POSTALCODE 10
#define MAX_RESIDENCE 41
#define MAX_COUNTRY 21
#define MAX_PHONE 21
#define MAX_HANDY 21
#define MAX_MAIL 41
#define MAX_BIRTHDAY 11

typedef struct
{
  char strNachname[MAX_NAME];
  char strVorname[MAX_FIRSTNAME];
  char strStrasse[MAX_STREET];
  char strPlz[MAX_POSTALCODE];
  char strWohnort[MAX_RESIDENCE];
  char strLand[MAX_COUNTRY];
  char strFestnetznr[MAX_PHONE];
  char strHandynr[MAX_HANDY];
  char strEmail[MAX_MAIL];
  char strBirthday[MAX_BIRTHDAY];
} AddressInfo;

///////////////////////////////////////////////////////////////
// Texts
char ADDRESS_TEXT_NAME[11]       = "Name";
char ADDRESS_TEXT_FIRSTNAME[11]  = "First Name";
char ADDRESS_TEXT_STREET[11]     = "Street";
char ADDRESS_TEXT_POSTALCODE[11] = "Code";
char ADDRESS_TEXT_RESIDENCE[11]  = "Residence";
char ADDRESS_TEXT_COUNTRY[9]     = "Country";
char ADDRESS_TEXT_PHONE[11]      = "Phone";
char ADDRESS_TEXT_HANDY[11]      = "Handy";
char ADDRESS_TEXT_MAIL[11]       = "E-Mail";
char ADDRESS_TEXT_BIRTHDAY[11]   = "Birthday";

char ADDRESS_TEXT_QUIT[7]   = "Quit";
char ADDRESS_TEXT_NEW[7]    = "New";
char ADDRESS_TEXT_DELETE[7] = "Delete";
char ADDRESS_TEXT_SHOW[7]   = "Show";
char ADDRESS_TEXT_OK[7]     = "Ok";
char ADDRESS_TEXT_EDIT[7]   = "Edit";
char ADDRESS_TEXT_CANCEL[7] = "Cancel";
char ADDRESS_TEXT_NEXT[7]   = "Next";

char ADDRESS_TEXT_RECORDS[11]   = "Records";
char ADDRESS_TEXT_RECORD[7]     = "Record";
char ADDRESS_TEXT_OF[4]         = "of";

char ADDRESS_TEXT_SHOW_ENTRY[31]   = "Show Entry";
char ADDRESS_TEXT_EDIT_ENTRY[31]   = "Edit Entry";
char ADDRESS_TEXT_NEW_ENTRY[31]    = "New Entry";

char ADDRESS_TEXT_MSG_CONTINUE[31]      = "Do u really want to continue ?";
char ADDRESS_TEXT_MSG_CHANGESLOST[31]   = "All changes will be lost.";
char ADDRESS_TEXT_MSG_DELETE1[31]       = "Do you really want to delete";
char ADDRESS_TEXT_MSG_DELETE2[31]       = "this record ?";
char ADDRESS_TEXT_MSG_PRESS_KEY[31]     = "Press key to continue !";
char ADDRESS_TEXT_MSG_SAVE[31]          = "Saving Data...";
char ADDRESS_TEXT_MSG_LOADERROR[31]     = "Error Loading Data !";
char ADDRESS_TEXT_MSG_BIRTHDAY_REMINDER[31] = "Birthday Reminder:";

///////////////////////////////////////////////////////////////

char textFile[400000] = "";
AddressInfo AllData[MAX_DATA];
int cntData = 0; // Anzahl der Datensaetze
int RangeBegin = 0;
int RangeEnd   = 0;
int Page = 1;    // Anzahl der Seiten
int CurrentEntry = 0;
bool g_fChanges = false;
int ShowMode = 0;

/*********************** PROTOTYPES ****************************************/
void ClearScreen();
void delay(unsigned int time);
void printNr(char* buffer, int nr, int withSign);
int Random(int min, int max);

int GetTime();
void DisplayDate();

void UpdateElement(int x, int y, int record, int element, int cntChars);
void ShowBuffer();
int get_CntData();
void ShowRange(int begin, int end, int ShowMode);
void ShowStatus();
void ShowEntryStatus();
void ShowMainMenu();
void ShowSpezialMenu(int kind);
void PlaceRowMarker();
void ShowCompleteEntry();
void ShowEntryMask(int kind);
void ShowNewEntry(int IsNew);
int SetUnderlinedCursor(int x, int y, int ascii);
AddressInfo SaveTempEntry(AddressInfo tmpAddress, int currentElement, int indexChar, int ascii);
int GetCurrentAscii(AddressInfo tmpAddress, int currentElement, int index, int oldascii);
int GetCharsOfElement(AddressInfo tmpAddress, int currentElement);
AddressInfo CopyDataEntry(AddressInfo ToCopy);
void DeleteEntry();
void DeleteChar(char* src, char*dest, int index);
int NextAscii(int lastAscii);
int PrevAscii(int lastAscii);
int colorRGB2Packed(int r, int g, int b);

// Sort-Functions
void quicksort(int lo, int hi, int element, int asc);
void exchange(int i, int j);

// File-Functions
int loadTextFile(char * fileName);
void WriteData(char * filename);
void ReadData(char * filename);
void ReadIniFile();

// Debug
void ShowAscii(int ascii);

// Messages
int ShowMessage(int type);
void RedBorderForMessage();


int  menu();
void entrymask();

int main()
{
//-----------------INIT OSD SYSTEM ------------------------------------------
    rtcInit();

    int startvalue = GetTime();

    srand(startvalue);

    //call osdInit and disable everything just to be safe
    osdInitA();
    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    spriteFont6x10.offset = 0;               //no need to use offset for font sprites
    spriteFont6x10.bytesPerLine = 1;	    //# of bytes horizontally in font source per char.
    spriteFont6x10.width = 6;                //actual width of one character in pixels
    spriteFont6x10.height = 10;               //actual height in pixels
    spriteFont6x10.bitsPerPixel = 1;         //each pixel in font source uses 1 bit (2-color)
    spriteFont6x10.bitsPerPixelShift = 0;    //use 0 since this is only 2-color sprite
    spriteFont6x10.transparent = 0;          //color 0 index in font source shall be transparent	                                                   //-1=no transparancy, 1 would print inverse text?
    					    //-1=not transparent so I can overwrite existing text
                                            //without having to "erase" is first...
    spriteFont6x10.pallette1 = 0;            //not necessary for font sprites
    spriteFont6x10.pallette2 = 0;            //ditto
    spriteFont6x10.pallette4 = 0;            //ditto
    spriteFont6x10.pallette8 = 0;            //ditto
    spriteFont6x10.pallette16 = (int**)&pal16;//point to our pal16 for printing on bitmap
    spriteFont6x10.pallette32 = (int**)&pal32;//point to our pal32 for printing on video


    // Hintergrund
    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;    //320pix wide * 2bytes per pixel
    screenBitmap.width = 320;             //width 320pix! (note it's divisible by 32, must be)
    screenBitmap.height = 240;            //height 500pix!
    screenBitmap.bitsPerPixelShift = 4; //use '4' for 16-bit graphics
    screenBitmap.bitsPerPixel = 16;     //must use 16 for bitmap planes

    osdSetComponentSizeA(OSD_BITMAP1, 640, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, 0);

    // Clear full Screen with black color
    graphicsBoxfA(&screenBitmap, 0, 0, 640, 240, ADDRESS_BACKCOLOR_SEPERATOR);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE | OSD_BITMAP_8BIT);


    screenBitmap2.offset = 0x03700000;
    screenBitmap2.bytesPerLine = 32;
    screenBitmap2.width = 8;
    screenBitmap2.height = 8;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;

    osdSetComponentSizeA(OSD_BITMAP2, 16, 8);
    osdSetComponentPositionA(OSD_BITMAP2, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP2, 0x03700000);
    osdSetComponentSourceWidthA(OSD_BITMAP2, 0x01);
    osdSetComponentConfigA(OSD_BITMAP2, 0);

    // Meldungen
    screenVideo.offset = 0x03c00000;
    screenVideo.bytesPerLine = 192*4;
    screenVideo.width = 192;
    screenVideo.height = 60;
    screenVideo.bitsPerPixelShift = 5;
    screenVideo.bitsPerPixel = 32;

    osdSetComponentSizeA(OSD_VIDEO1, 384, 60);
    osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_VIDEO1, 0x03c00000);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x18);
    osdSetComponentConfigA(OSD_VIDEO1, 0);

    graphicsBoxfA(&screenVideo, 0, 0, 384, 60, 0x005dc48b);

    int res = 1;

    while(res !=  0)
    {
		res = menu();

		if(res > 0)
		{
		}
    }

    return;
}

int menu()
{
   ReadIniFile();

   ReadData("/ABOOK/ADDRESS.CSV");//("/ADDRESS.CSV");

   RangeBegin = 0;
   RangeEnd   = 15;

	// Selectionbox
    graphicsBoxfA(&screenBitmap2, 0, 0, 16, 8, ADDRESS_BACKCOLOR_MARKER);

CompleteUpdate:

   ClearScreen();
   pal16[1] = ADDRESS_TEXTCOLOR_HEAD;
   graphicsBoxfA(&screenBitmap, 0, 0, 640, 20, ADDRESS_BACKCOLOR_HEAD);
   graphicsStringA(&screenBitmap, 10, 5, &spriteFont6x10, std6x10_, 6, 0, "ADDRESS BOOK V1.33 by SCHOKI");
   graphicsBoxfA(&screenBitmap, 0, 21, 595, 2, ADDRESS_BACKCOLOR_SEPERATOR); // schwarzer Schatten
   graphicsBoxfA(&screenBitmap, 0, 23, 595, 297, ADDRESS_BACKCOLOR_LIST); // hellblau
   DisplayDate();

   ShowMainMenu();

StartLabel:

   ShowRange(RangeBegin, RangeEnd, ShowMode);
   ShowStatus();
   PlaceRowMarker();

   int key = 0;
   for(;;)
   {
      key = buttonsGetStatusA();          // CHECK buttons

      if (key & BUTTONS_AV300_MENU1)
      {   //F1 button - New
         ShowNewEntry(1);
         delay(0x15000);
         goto CompleteUpdate;
      }

      if (key & BUTTONS_AV300_MENU3)
      {
         if(get_CntData() > 0)
			{
				ShowCompleteEntry();
			}
         delay(0x15000);
         goto CompleteUpdate;
      }

      if (key & BUTTONS_AV300_MENU2)
      {  //F2 button - Delete
		   int fRet = 0;
         delay(0x15000);

			fRet = ShowMessage(1);
			if(fRet == RET_OK)
			{
			   // Loeschen aus Datenbestand
				DeleteEntry();
            g_fChanges = true; // Aenderungen beim Schliessen speichern
			}

         ShowMainMenu();
			goto StartLabel;
      }

      if (key & BUTTONS_AV300_LEFT)
      {
         if(Page > 1)
			{
				Page--;
				RangeBegin-=MAX_ENTRIES_PAGE;
				RangeEnd-=MAX_ENTRIES_PAGE;
				CurrentEntry=0;
				delay(0x15000);
				goto StartLabel;
			}
      }

      if (key & BUTTONS_AV300_RIGHT)
      {
         if(RangeEnd/*+1*/ < get_CntData())
			{
				// es gibt noch weitere Datensaetze
						// next page
				Page++;
				RangeBegin+=MAX_ENTRIES_PAGE;
				RangeEnd+=MAX_ENTRIES_PAGE;
				CurrentEntry=0;
				delay(0x15000);
				goto StartLabel;
			}
      }

      if (key & BUTTONS_AV300_DOWN)
      {
         if(RangeBegin+CurrentEntry+1 < get_CntData())
			{
				// es gibt noch weitere Datensaetze

				if(CurrentEntry+1 > MAX_ENTRIES_PAGE-1)
				{
					// next page
					Page++;
					RangeBegin+=MAX_ENTRIES_PAGE;
					RangeEnd+=MAX_ENTRIES_PAGE;
					CurrentEntry=0;
					goto StartLabel;
				}
				else
				{
					CurrentEntry++;
				}

				PlaceRowMarker();
			}
         delay(0x15000);
      }

      if (key & BUTTONS_AV300_UP)
      {
         if(Page > 1)
			{
				if(CurrentEntry-1 < 0)
				{
					Page--;
					RangeBegin-=MAX_ENTRIES_PAGE;
					RangeEnd-=MAX_ENTRIES_PAGE;
					CurrentEntry=MAX_ENTRIES_PAGE-1;
					goto StartLabel;
				}
				else
				{
					CurrentEntry--;
				}
			}
			else
			{
				if(CurrentEntry-1 >= 0)
				{
					CurrentEntry--;
				}
				else
				{
					// nothing
				}
			}

         PlaceRowMarker();

         delay(0x15000);
      }

      if (key & BUTTONS_AV300_ON)
      {
         if(ShowMode == 0)
			{
			   ShowMode = 1;
            quicksort(0, cntData-1, 9, 1);
			}
		   else
			{
				ShowMode = 0;
            quicksort(0, cntData-1, 0, 1);
			}

         delay(0x15000);
   		goto StartLabel;
      }

      if (key & BUTTONS_AV300_OFF)
      {
         if(g_fChanges == true)
			{
            quicksort(0, cntData-1, 0, 1);
            WriteData("/ABOOK/ADDRESS.CSV");
			}

         return 0;
      }
   }

   return 0;
}

void ShowMainMenu()
{
   pal16[1] = ADDRESS_TEXTCOLOR_MENU;
   graphicsBoxfA(&screenBitmap, 595, 22, 44, 298, ADDRESS_BACKCOLOR_MENU);

   graphicsStringA(&screenBitmap, 600, 42, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_QUIT);

   graphicsStringA(&screenBitmap, 600,150, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_NEW);

   graphicsStringA(&screenBitmap, 600,180, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_DELETE);

   graphicsStringA(&screenBitmap, 600,210, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_SHOW);
}

void DeleteEntry()
{
   int i = 0;

	if(cntData > 0)
	{
		// Die Daten hinter dem aktuellen Datensatz eine Position nach vorn kopieren
		// und die Anzahl um eins verkleinern
      if(RangeBegin+CurrentEntry+1 < get_CntData())
		{
			for(i = RangeBegin+CurrentEntry+1; i < cntData; i++)
			{
				exchange( i, i-1 );
			}
      }

		// Den Speicherbereich leermachen am Ende
		memset(&AllData[cntData],0,sizeof(AddressInfo));

		cntData--;
	}
}

void ShowNewEntry(int IsNew)
{
   int ascii = ASCII_DEFAULT;
   int xPos = 70;
   int yPos = 31;
   int indexChar = 0;
   int currentElement = 0;
	int cntKeyUpDown = 0;
   bool fUpKey = false;
   bool fDownKey = false;
   bool fValidEntry = false; // wurde ueberhaupt ein Buchstabe ausgewaehlt ?
	int cntElementChars = 0; // Anzahl der schon vorhandenen Zeichen im aktuellen Element

   AddressInfo tmpAddress;
   memset(&tmpAddress, 0, sizeof(AddressInfo));

   osdSetComponentConfigA(OSD_BITMAP2, 0);

	if(IsNew == 1)
	{
      ShowEntryMask(1);
   	pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette

		// Nachname
		graphicsBoxfA(&screenBitmap, 65, 30, 10 + (MAX_NAME-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Vorname
		graphicsBoxfA(&screenBitmap, 65, 50, 10 + (MAX_FIRSTNAME-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Street
		graphicsBoxfA(&screenBitmap, 65, 70, 10 + (MAX_STREET-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//PLZ
		graphicsBoxfA(&screenBitmap, 65, 90, 10 + (MAX_POSTALCODE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Land
		graphicsBoxfA(&screenBitmap, 185, 90, 10 + (MAX_COUNTRY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Wohnort
		graphicsBoxfA(&screenBitmap, 65, 110, 10 + (MAX_RESIDENCE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Mail
		graphicsBoxfA(&screenBitmap, 65, 130, 10 + (MAX_MAIL-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Phone
		graphicsBoxfA(&screenBitmap, 65, 150, 10 + (MAX_PHONE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Handy
		graphicsBoxfA(&screenBitmap, 65, 170, 10 + (MAX_HANDY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);

		//Birthday
		graphicsBoxfA(&screenBitmap, 65, 190, 10 + (MAX_BIRTHDAY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   }
	else
	{
	   // Edit in die Ueberschrift schreiben
		pal16[1] = ADDRESS_TEXTCOLOR_HEAD;
		graphicsBoxfA(&screenBitmap, 0, 0, 640, 20, ADDRESS_BACKCOLOR_HEAD);
      graphicsStringA(&screenBitmap, 10, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_EDIT_ENTRY);

   	ShowSpezialMenu(2);

   	pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;

		// aktuellen Datensatz in die temp. Struktur schreiben
      tmpAddress = CopyDataEntry(AllData[RangeBegin+CurrentEntry]);
	}

 NextElement:

   ascii = GetCurrentAscii(tmpAddress, currentElement, indexChar, ascii);

 NextChar:

   SetUnderlinedCursor(xPos, yPos, ascii);

   int key = 0;

   for(;;)
   {
      key = buttonsGetStatusA();          // CHECK buttons

      if (key & BUTTONS_AV300_OFF)
      {  //OFF button : Delete the current char
		   char tmpChar[MAX_ELEMENT_CHARS];
			if(currentElement == 0)
			{
			   if(strlen(tmpAddress.strNachname) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strNachname, tmpChar, indexChar);
				strcpy(tmpAddress.strNachname, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 30, 10 + (MAX_NAME-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 31, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strNachname);
			}
			else if(currentElement == 1)
			{
			   if(strlen(tmpAddress.strVorname) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strVorname, tmpChar, indexChar);
				strcpy(tmpAddress.strVorname, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 50, 10 + (MAX_FIRSTNAME-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 51, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strVorname);
			}
			else if(currentElement == 2)
			{
			   if(strlen(tmpAddress.strStrasse) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strStrasse, tmpChar, indexChar);
				strcpy(tmpAddress.strStrasse, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 70, 10 + (MAX_STREET-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 71, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strStrasse);
			}
			else if(currentElement == 3)
			{
			   if(strlen(tmpAddress.strPlz) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strPlz, tmpChar, indexChar);
				strcpy(tmpAddress.strPlz, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 90, 10 + (MAX_POSTALCODE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 91, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strPlz);
			}
			else if(currentElement == 4)
			{
			   if(strlen(tmpAddress.strLand) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
				DeleteChar(tmpAddress.strLand, tmpChar, indexChar);
				strcpy(tmpAddress.strLand, tmpChar);
      		graphicsBoxfA(&screenBitmap, 185, 90, 10 + (MAX_COUNTRY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 190, 91, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strLand);
			}
			else if(currentElement == 5)
			{
			   if(strlen(tmpAddress.strWohnort) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strWohnort, tmpChar, indexChar);
				strcpy(tmpAddress.strWohnort, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 110, 10 + (MAX_RESIDENCE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 111, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strWohnort);
			}
			else if(currentElement == 6)
			{
			   if(strlen(tmpAddress.strEmail) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strEmail, tmpChar, indexChar);
				strcpy(tmpAddress.strEmail, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 130, 10 + (MAX_MAIL-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 131, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strEmail);
			}
			else if(currentElement == 7)
			{
			   if(strlen(tmpAddress.strFestnetznr) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strFestnetznr, tmpChar, indexChar);
				strcpy(tmpAddress.strFestnetznr, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 150, 10 + (MAX_PHONE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 151, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strFestnetznr);
			}
			else if(currentElement == 8)
			{
			   if(strlen(tmpAddress.strHandynr) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strHandynr, tmpChar, indexChar);
				strcpy(tmpAddress.strHandynr, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 170, 10 + (MAX_HANDY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 171, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strHandynr);
			}
			else if(currentElement == 9)
			{
			   if(strlen(tmpAddress.strBirthday) == 1)
				   fValidEntry = false; //Wenn auch das letzte Zeichen gel?scht, wird den Status umsetzen
            DeleteChar(tmpAddress.strBirthday, tmpChar, indexChar);
				strcpy(tmpAddress.strBirthday, tmpChar);
      		graphicsBoxfA(&screenBitmap, 65, 190, 10 + (MAX_BIRTHDAY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
            graphicsStringA(&screenBitmap, 70, 191, &spriteFont6x10, std6x10_, 6, 0, tmpAddress.strBirthday);
			}

         delay(0x15000);
			goto NextElement;
		}

      if (key & BUTTONS_AV300_MENU1)
      {  //F1 button : ok
         fUpKey = false;
         fDownKey = false;

         tmpAddress = SaveTempEntry(tmpAddress, currentElement, indexChar, ascii);

			if(IsNew == 1)
			{
            AllData[cntData] = CopyDataEntry(tmpAddress);

				if(cntData < MAX_DATA)
				{
					cntData++;
				}
			}
         else
            AllData[RangeBegin+CurrentEntry] = CopyDataEntry(tmpAddress);

         g_fChanges = true; // Aenderungen beim Schliessen speichern
         delay(0x15000);

         break;
      }
      else if (key & BUTTONS_AV300_MENU3)
      {   //F3 button : next element
         fUpKey = false;
         fDownKey = false;

         cntElementChars = GetCharsOfElement(tmpAddress, currentElement);

	      if( (fValidEntry == false) && (cntElementChars == 0) )
			{
				ascii = 32; // Wenn nichts eingegeben wurde, dann ein Leerzeichen eintragen
				graphicsBoxfA(&screenBitmap, xPos, yPos, 6, 11, ADDRESS_BACKCOLOR_ELEMENTENTRY); // weisser Hintergrund-Anzeige updaten
			}

	      if(fValidEntry == true)
			{
            tmpAddress = SaveTempEntry(tmpAddress, currentElement, indexChar, ascii);
			}

         graphicsBoxfA(&screenBitmap, xPos, yPos+10, 6, 1, ADDRESS_BACKCOLOR_ELEMENTENTRY); // schwarze Markierung wegnehmen

    		fValidEntry = false; // wieder zuruecksetzen

         currentElement++;

			if(currentElement == MAX_ELEMENTS)
			{
			   // nach vorn springen
				currentElement = 0;
				xPos = 70;
				yPos = 31;
			}
			else if(currentElement == 4)
			{
				// Spezialfall 2 in einer Reihe
				xPos = 190;
			}
			else
			{
			   xPos = 70;
				yPos += 20;
			}

			indexChar = 0;

         delay(0x30000);
         goto NextElement;
      }
      else if (key & BUTTONS_AV300_MENU2)
      {   //F2 button : Cancel
		   int retVal = 0;
         delay(0x15000);
         retVal = ShowMessage(2); // Warnung das Daten verloren gehen
			if(retVal == RET_OK)
			{
            break;
			}
			else
			{
            ShowSpezialMenu(2);
            pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;
			}
      }
      // Buchstaben waehlen
      else if (key & BUTTONS_AV300_UP)
      {
         fDownKey = false;
         fValidEntry = true;

         ascii = NextAscii(ascii); // definierte Reihenfolge

			if(fUpKey == true)
			{
			   if(cntKeyUpDown > 5)
				   delay(0x5000);

				else
				   delay(0x10000);

            cntKeyUpDown++;
			}
			else
				delay(0x20000);

			fUpKey = true;
    		goto NextChar;
		}
		else if (key & BUTTONS_AV300_DOWN)
		{
			fUpKey = false;
			fValidEntry = true;

			ascii = PrevAscii(ascii);

			if(fDownKey == true)
			{
			   if(cntKeyUpDown > 5)
				   delay(0x5000);
				else
				   delay(0x10000);

            cntKeyUpDown++;
			}
			else
				delay(0x20000);

			fDownKey = true;
			goto NextChar;
      }
      // Zum naechsten Zeichen wechseln
      else if (key & BUTTONS_AV300_LEFT)
      {
         fUpKey = false;
         fDownKey = false;
         fValidEntry = true;

         if(indexChar > 0)
			{
				tmpAddress = SaveTempEntry(tmpAddress, currentElement, indexChar, ascii);

				graphicsBoxfA(&screenBitmap, xPos, yPos+10, 6, 1, ADDRESS_BACKCOLOR_ELEMENTENTRY);
				xPos-=6;
				indexChar--;
				delay(0x20000);
				goto NextElement;
			}
      }
      else if (key & BUTTONS_AV300_RIGHT)
      {
         fUpKey = false;
         fDownKey = false;
         fValidEntry = true;

         int MaxValue = 0;

         if(currentElement == 0)
				MaxValue = MAX_NAME;
			else if(currentElement == 1)
				MaxValue = MAX_FIRSTNAME;
			else if(currentElement == 2)
				MaxValue = MAX_STREET;
			else if(currentElement == 3)
				MaxValue = MAX_POSTALCODE;
			else if(currentElement == 4)
				MaxValue = MAX_COUNTRY;
			else if(currentElement == 5)
				MaxValue = MAX_RESIDENCE;
			else if(currentElement == 6)
   			MaxValue = MAX_MAIL;
			else if(currentElement == 7)
				MaxValue = MAX_PHONE;
			else if(currentElement == 8)
				MaxValue = MAX_HANDY;
			else if(currentElement == 9)
				MaxValue = MAX_BIRTHDAY;

         if(indexChar+1 < MaxValue-1)
         {
            tmpAddress = SaveTempEntry(tmpAddress, currentElement, indexChar, ascii);

            graphicsBoxfA(&screenBitmap, xPos, yPos+10, 6, 1, ADDRESS_BACKCOLOR_ELEMENTENTRY); // schwarzer Markierung wegnehmen
            xPos+=6;
            indexChar++;
            delay(0x20000);
	         goto NextElement;
	      }
      }
      else
      {
         fUpKey = false;
         fDownKey = false;
			cntKeyUpDown = 0;
      }
   }
}

int GetCharsOfElement(AddressInfo tmpAddress, int currentElement)
{
   int cntElementChars = 0;

	if(currentElement == 0)
		cntElementChars = strlen(tmpAddress.strNachname);
	else if(currentElement == 1)
		cntElementChars = strlen(tmpAddress.strVorname);
	else if(currentElement == 2)
		cntElementChars = strlen(tmpAddress.strStrasse);
	else if(currentElement == 3)
		cntElementChars = strlen(tmpAddress.strPlz);
	else if(currentElement == 4)
		cntElementChars = strlen(tmpAddress.strLand);
	else if(currentElement == 5)
		cntElementChars = strlen(tmpAddress.strWohnort);
	else if(currentElement == 6)
		cntElementChars = strlen(tmpAddress.strEmail);
	else if(currentElement == 7)
		cntElementChars = strlen(tmpAddress.strFestnetznr);
	else if(currentElement == 8)
		cntElementChars = strlen(tmpAddress.strHandynr);
	else if(currentElement == 9)
		cntElementChars = strlen(tmpAddress.strBirthday);

	return cntElementChars;
}

AddressInfo SaveTempEntry(AddressInfo tmpAddress, int currentElement, int indexChar, int ascii)
{
   char temp[1];
   temp[0] = ascii;
//   temp[1] = '\0';
   int len = 0;

   AddressInfo tmp;
	memset(&tmp, 0, sizeof(AddressInfo));
   tmp = CopyDataEntry(tmpAddress);

   if(currentElement == 0)
	{
	   len = strlen(tmp.strNachname);
      tmp.strNachname[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strNachname[indexChar+1] = '\0';
	}
   else if(currentElement == 1)
	{
	   len = strlen(tmp.strVorname);
      tmp.strVorname[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strVorname[indexChar+1] = '\0';
	}
   else if(currentElement == 2)
	{
	   len = strlen(tmp.strStrasse);
      tmp.strStrasse[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strStrasse[indexChar+1] = '\0';
	}
   else if(currentElement == 3)
	{
	   len = strlen(tmp.strPlz);
      tmp.strPlz[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strPlz[indexChar+1] = '\0';
	}
   else if(currentElement == 4)
	{
	   len = strlen(tmp.strLand);
      tmp.strLand[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strLand[indexChar+1] = '\0';
	}
   else if(currentElement == 5)
	{
	   len = strlen(tmp.strWohnort);
      tmp.strWohnort[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strWohnort[indexChar+1] = '\0';
	}
   else if(currentElement == 6)
	{
	   len = strlen(tmp.strEmail);
      tmp.strEmail[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strEmail[indexChar+1] = '\0';
	}
   else if(currentElement == 7)
	{
	   len = strlen(tmp.strFestnetznr);
      tmp.strFestnetznr[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strFestnetznr[indexChar+1] = '\0';
	}
   else if(currentElement == 8)
	{
	   len = strlen(tmp.strHandynr);
      tmp.strHandynr[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strHandynr[indexChar+1] = '\0';
	}
   else if(currentElement == 9)
	{
	   len = strlen(tmp.strBirthday);
      tmp.strBirthday[indexChar] = ascii;
	   if(indexChar >= len)
         tmp.strBirthday[indexChar+1] = '\0';
	}

   return tmp;
}

void ShowCompleteEntry()
{
   osdSetComponentConfigA(OSD_BITMAP2, 0);

ShowAgain:

   ShowEntryMask(0);

   ShowEntryStatus();

   //////////////////////
   // Inhalte schreiben
   //////////////////////

updateEntry:

   //Name
   graphicsBoxfA(&screenBitmap, 65, 30, 10 + (MAX_NAME-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 31, RangeBegin+CurrentEntry, 0, 0);

   //Vorname
   graphicsBoxfA(&screenBitmap, 65, 50, 10 + (MAX_FIRSTNAME-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 51, RangeBegin+CurrentEntry, 1, 0);

   //Street
   graphicsBoxfA(&screenBitmap, 65, 70, 10 + (MAX_STREET-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 71, RangeBegin+CurrentEntry, 2, 0);

   //PLZ
   graphicsBoxfA(&screenBitmap, 65, 90, 10 + (MAX_POSTALCODE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 91, RangeBegin+CurrentEntry, 3, 0);

   //Land
   graphicsBoxfA(&screenBitmap, 185, 90, 10 + (MAX_COUNTRY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(190, 91, RangeBegin+CurrentEntry, 4, 0);

   //Wohnort
   graphicsBoxfA(&screenBitmap, 65, 110, 10 + (MAX_RESIDENCE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 111, RangeBegin+CurrentEntry, 5, 0);

   //Mail
   graphicsBoxfA(&screenBitmap, 65, 130, 10 + (MAX_MAIL-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 131, RangeBegin+CurrentEntry, 6, 0);

   //Phone
   graphicsBoxfA(&screenBitmap, 65, 150, 10 + (MAX_PHONE-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 151, RangeBegin+CurrentEntry, 7, 0);

   //Handy
   graphicsBoxfA(&screenBitmap, 65, 170, 10 + (MAX_HANDY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 171, RangeBegin+CurrentEntry, 8, 0);

   //Birthday
   graphicsBoxfA(&screenBitmap, 65, 190, 10 + (MAX_BIRTHDAY-1)*6, 12, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTENTRY;  //0x00 is black in the built-in 256-color pallette
   UpdateElement(70, 191, RangeBegin+CurrentEntry, 9, 0);

   int key = 0;

   for(;;)
   {
      key = buttonsGetStatusA();          // CHECK buttons
      if (key & BUTTONS_AV300_MENU1)
      {   //F1 button : goto Mainmenu
         break;
      }

      if (key & BUTTONS_AV300_MENU2)
      {  //F2 button - Delete
		   int fRet = 0;
         delay(0x15000);

			fRet = ShowMessage(5);
			if(fRet == RET_OK)
			{
			   // Loeschen aus Datenbestand
				DeleteEntry();
            g_fChanges = true; // Aenderungen beim Schliessen speichern
			}

			goto ShowAgain;
      }

      if (key & BUTTONS_AV300_MENU3)
      {   //F3 button : Edit
          delay(0x15000);
          ShowNewEntry(0);
          delay(0x15000);
			 goto ShowAgain;
      }

      if (key & BUTTONS_AV300_LEFT)
      {
         if(Page > 1)
			{
				if(CurrentEntry-1 < 0)
				{
					Page--;
					RangeBegin-=MAX_ENTRIES_PAGE;
					RangeEnd-=MAX_ENTRIES_PAGE;
					CurrentEntry=MAX_ENTRIES_PAGE-1;
				}
				else
				{
					CurrentEntry--;
				}

				ShowEntryStatus();
				delay(0x15000);
				goto updateEntry;
			}
			else
			{
				if(CurrentEntry-1 >= 0)
				{
					CurrentEntry--;

					ShowEntryStatus();
					delay(0x15000);
					goto updateEntry;
				}
				else
				{
					// nothing
				}
			}
      }

      if (key & BUTTONS_AV300_RIGHT)
      {
         if(RangeBegin+CurrentEntry+1 < get_CntData())
	      {
				// es gibt noch weitere Datensaetze

				if(CurrentEntry+1 >= MAX_ENTRIES_PAGE)
				{
					// next page
					Page++;
					RangeBegin+=MAX_ENTRIES_PAGE;
					RangeEnd+=MAX_ENTRIES_PAGE;
					CurrentEntry=0;
				}
				else
				{
					CurrentEntry++;
				}

				ShowEntryStatus();
				delay(0x15000);

				goto updateEntry;
			}
      }
   }
}

void ShowEntryMask(int kind)
{
   // Headline
   if(kind == 0)
	{
      ClearScreen();
		pal16[1] = ADDRESS_TEXTCOLOR_HEAD;  //0xff is white in the built-in 256-color pallette
		graphicsBoxfA(&screenBitmap, 0, 0, 640, 20, ADDRESS_BACKCOLOR_HEAD);
      graphicsStringA(&screenBitmap, 10, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_SHOW_ENTRY);
   }
	else if(kind == 2)
	{
		pal16[1] = ADDRESS_TEXTCOLOR_HEAD;  //0xff is white in the built-in 256-color pallette
		graphicsBoxfA(&screenBitmap, 0, 0, 640, 20, ADDRESS_BACKCOLOR_HEAD);
      graphicsStringA(&screenBitmap, 10, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_EDIT_ENTRY);
	}
	else if(kind == 3)
	{
		pal16[1] = ADDRESS_TEXTCOLOR_HEAD;  //0xff is white in the built-in 256-color pallette
		graphicsBoxfA(&screenBitmap, 0, 0, 640, 20, ADDRESS_BACKCOLOR_HEAD);
      graphicsStringA(&screenBitmap, 10, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_SHOW_ENTRY);
	}
   else
	{
      ClearScreen();
		pal16[1] = ADDRESS_TEXTCOLOR_HEAD;  //0xff is white in the built-in 256-color pallette
		graphicsBoxfA(&screenBitmap, 0, 0, 640, 20, ADDRESS_BACKCOLOR_HEAD);
      graphicsStringA(&screenBitmap, 10, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_NEW_ENTRY);
	}

   graphicsBoxfA(&screenBitmap, 0, 21, 640, 2, ADDRESS_BACKCOLOR_SEPERATOR); // schwarzer Schatten
   graphicsBoxfA(&screenBitmap, 0, 23, 640, 297, ADDRESS_BACKCOLOR_LIST); // hellblau

	pal16[1] = ADDRESS_TEXTCOLOR_ELEMENTNAME;

	if((kind != 2)&&(kind != 3))
	{
		//Name
		graphicsStringA(&screenBitmap, 10,30, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_NAME);

		//Vorname
		graphicsStringA(&screenBitmap, 10,50, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_FIRSTNAME);

		//Street
		graphicsStringA(&screenBitmap, 10,70, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_STREET);

		//PLZ
		graphicsStringA(&screenBitmap, 10,90, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_POSTALCODE);

		//Land
		graphicsStringA(&screenBitmap, 135,90, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_COUNTRY);

		//Wohnort
		graphicsStringA(&screenBitmap, 10,110, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_RESIDENCE);

		//Mail
		graphicsStringA(&screenBitmap, 10,130, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_MAIL);

		//Phone
		graphicsStringA(&screenBitmap, 10,150, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_PHONE);

		//Handy
		graphicsStringA(&screenBitmap, 10,170, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_HANDY);

		//Birthday
		graphicsStringA(&screenBitmap, 10,190, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_BIRTHDAY);
   }

	ShowSpezialMenu(kind);
}

void ShowSpezialMenu(int kind)
{
   graphicsBoxfA(&screenBitmap, 595, 145, 44, 298, ADDRESS_BACKCOLOR_MENU); // dunkelblau
   pal16[1] = ADDRESS_TEXTCOLOR_MENU;  //0xfb is yellow in the built-in 256-color pallette

   if((kind == 0) || (kind == 3))
   {
      graphicsStringA(&screenBitmap, 600,180, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_DELETE);
      graphicsStringA(&screenBitmap, 600,210, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_EDIT);
   }
   else
   {
      graphicsStringA(&screenBitmap, 600,180, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_CANCEL);
      graphicsStringA(&screenBitmap, 600,210, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_NEXT);
   }

   graphicsStringA(&screenBitmap, 600,150, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_OK);
}

int loadTextFile(char * fileName)
{
   char str[10];

	inifile();
	inidir();
	inifatinfo();

	ataSelectHDDA();
//	ataPowerUpHDDA();

   memset(&textFile,   0, sizeof(textFile));

	ataReadMBR();
	int farret = fatInit(getPartition(0));

	int fileHandle = fopen(fileName,"r+");

	if (fileHandle >= 0)
	{
		int fileSize = fsize(fileHandle);
		if (fileSize >= sizeof(textFile))
		{
			fileSize = sizeof(textFile);
	   }

		if (fileSize > 0)
		{
			fread(fileHandle, textFile, fileSize);
			textFile[fileSize] = 0;
		}

	   fclose(fileHandle);
   }

   if( (fileHandle < 0) || (farret != 0) )
   {
      ShowMessage(4);

		pal32[1] = ADDRESS_TEXTCOLOR_MESSAGE;
		printNr(str, farret, 1);
		graphicsStringA(&screenVideo, 10, 30, &spriteFont6x10, std6x10_, 5, 0, "Fat:");
		graphicsStringA(&screenVideo, 50, 30, &spriteFont6x10, std6x10_, 5, 0, str);

		printNr(str, fileHandle, 1);
		graphicsStringA(&screenVideo, 100, 30, &spriteFont6x10, std6x10_, 5, 0, "Handle:");
		graphicsStringA(&screenVideo, 150, 30, &spriteFont6x10, std6x10_, 5, 0, str);

	   // Im Fehlerfall das Fenster mit den Fehlerwerten bis zum naechsten Tastendruck anzeigen
      graphicsStringA(&screenVideo, 40, 50, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_MSG_PRESS_KEY);

		int key = 0;

		for(;;)
		{
			key = buttonsGetStatusA();          // CHECK buttons
			if (key & BUTTONS_AV300_ANY)
			{
				break;
			}
		}
   }

// Might as well power down HD and save battery :)
//   ataPowerDownHDDA();
	osdSetComponentConfigA(OSD_VIDEO1, 0);

   return 1;
}

void WriteData(char * filename)
{
   int ret = 0;
   int i = 0;
   int j = 0;

   char str[10];

   ShowMessage(0);

   inifile();
   inidir();
   inifatinfo(0);

   ataSelectHDDA();
//	ataPowerUpHDDA();

   ataReadMBR();
   int farret = fatInit(getPartition(0));
   printNr(str, farret, 1);

   pal32[1] = ADDRESS_TEXTCOLOR_MESSAGE;
   graphicsStringA(&screenVideo, 10, 30, &spriteFont6x10, std6x10_, 5, 0, "Fat:");
   graphicsStringA(&screenVideo, 50, 30, &spriteFont6x10, std6x10_, 5, 0, str);

   int fileHandle = 0;

   fileHandle = fopen(filename,"w");

   printNr(str, fileHandle, 1);
   graphicsStringA(&screenVideo, 100, 30, &spriteFont6x10, std6x10_, 5, 0, "Handle:");
   graphicsStringA(&screenVideo, 150, 30, &spriteFont6x10, std6x10_, 5, 0, str);

   if (fileHandle >= 0)
   {
      // Daten schreiben
      for(i = 0; i < cntData; i++)
      {
         for(j = 0; j < MAX_ELEMENTS; j++)
         {
            if(j == 0)
	         {
               fwrite(fileHandle, AllData[i].strNachname, strlen(AllData[i].strNachname));
               fwrite(fileHandle, ";", 1);
	         }
            else if(j == 1)
	         {
               fwrite(fileHandle, AllData[i].strVorname, strlen(AllData[i].strVorname));
               fwrite(fileHandle, ";", 1);
            }
            else if(j == 2)
				{
							fwrite(fileHandle, AllData[i].strStrasse, strlen(AllData[i].strStrasse));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 3)
				{
							fwrite(fileHandle, AllData[i].strPlz, strlen(AllData[i].strPlz));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 4)
				{
							fwrite(fileHandle, AllData[i].strWohnort, strlen(AllData[i].strWohnort));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 5)
				{
							fwrite(fileHandle, AllData[i].strLand, strlen(AllData[i].strLand));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 6)
				{
							fwrite(fileHandle, AllData[i].strFestnetznr, strlen(AllData[i].strFestnetznr));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 7)
				{
							fwrite(fileHandle, AllData[i].strHandynr, strlen(AllData[i].strHandynr));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 8)
				{
							fwrite(fileHandle, AllData[i].strEmail, strlen(AllData[i].strEmail));
							fwrite(fileHandle, ";", 1);
				}
						else if(j == 9)
				{
							fwrite(fileHandle, AllData[i].strBirthday, strlen(AllData[i].strBirthday));
							fwrite(fileHandle, "\n", 1);
				}
			}
      }

      fclose(fileHandle);
   }

   if( (fileHandle < 0) || (farret != 0) )
   {
	   // Im Fehlerfall das Fenster mit den Fehlerwerten bis zum naechsten Tastendruck anzeigen
      graphicsStringA(&screenVideo, 40, 50, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_MSG_PRESS_KEY);

		int key = 0;

		for(;;)
		{
			key = buttonsGetStatusA();          // CHECK buttons
			if (key & BUTTONS_AV300_ANY)
			{
				break;
			}
		}
   }

//    ataPowerDownHDDA();
	osdSetComponentConfigA(OSD_VIDEO1, 0);
}

void ReadData(char * filename)
{
   char str[10];
   int cntElement = 0;
   char* pbuffer = textFile;
   char *p2;
   int yPos = 0;
   int xPos = 0;
   int y = 0;
   int ret = 0;
   int cnt = 0;
	int i = 0;

   ret = loadTextFile(filename);

   if(ret == 0) return;

   while(*pbuffer != 0)
   {
      p2 = pbuffer;
      cnt =0;

      while(*p2 != ';' && *p2 != '\n' && *p2 != 0)  // Trennzeichen suchen
      {
         p2++;

         cnt++;
      }

      if(*p2 == 0)
         break;

      if(cntElement == 0)
      {
	      if(cnt >= MAX_NAME-1) cnt = MAX_NAME-1;
	      strncpy(AllData[cntData].strNachname, pbuffer, cnt);
         AllData[cntData].strNachname[cnt]='\0';
      }
      else if(cntElement == 1)
      {
         if(cnt >= MAX_FIRSTNAME-1) cnt = MAX_FIRSTNAME-1;
         strncpy(AllData[cntData].strVorname, pbuffer, cnt);
         AllData[cntData].strVorname[cnt]='\0';
      }
      else if(cntElement == 2)
      {
         if(cnt >= MAX_STREET-1) cnt = MAX_STREET-1;
         strncpy(AllData[cntData].strStrasse, pbuffer, cnt);
         AllData[cntData].strStrasse[cnt]='\0';
      }
      else if(cntElement == 3)
      {
         if(cnt >= MAX_POSTALCODE-1) cnt = MAX_POSTALCODE-1;
         strncpy(AllData[cntData].strPlz, pbuffer, cnt);
         AllData[cntData].strPlz[cnt]='\0';
      }
      else if(cntElement == 4)
      {
         if(cnt >= MAX_RESIDENCE-1) cnt = MAX_RESIDENCE-1;
         strncpy(AllData[cntData].strWohnort, pbuffer, cnt);
         AllData[cntData].strWohnort[cnt]='\0';
      }
      else if(cntElement == 5)
      {
         if(cnt >= MAX_COUNTRY-1) cnt = MAX_COUNTRY-1;
         strncpy(AllData[cntData].strLand, pbuffer, cnt);
         AllData[cntData].strLand[cnt]='\0';
      }
      else if(cntElement == 6)
      {
         if(cnt >= MAX_PHONE-1) cnt = MAX_PHONE-1;
         strncpy(AllData[cntData].strFestnetznr, pbuffer, cnt);
         AllData[cntData].strFestnetznr[cnt]='\0';
      }
      else if(cntElement == 7)
      {
         if(cnt >= MAX_HANDY-1) cnt = MAX_HANDY-1;
         strncpy(AllData[cntData].strHandynr, pbuffer, cnt);
         AllData[cntData].strHandynr[cnt]='\0';
      }
      else if(cntElement == 8)
      {
         if(cnt >= MAX_MAIL-1) cnt = MAX_MAIL-1;
         strncpy(AllData[cntData].strEmail, pbuffer, cnt);
         AllData[cntData].strEmail[cnt]='\0';
      }
      else if(cntElement == 9)
      {
         if(cnt >= MAX_BIRTHDAY-1) cnt = MAX_BIRTHDAY-1;
         strncpy(AllData[cntData].strBirthday, pbuffer, cnt);
         AllData[cntData].strBirthday[cnt]='\0';
      }

      if(*p2 == ';') // neues Element ?
      {
         cntElement++;
      }

      if(*p2 == '\n') // neuer Datensatz ?
      {
//         printNr(str, cntData, 1);
//         graphicsStringA(&screenBitmap, 10+xPos, 110+yPos, &spriteFont6x10, std6x10_, 5, 0, str);
//         yPos=0;
//	 xPos+=40;

         cntElement = 0;
         cntData++;
      }

      pbuffer = p2+1;
   }

   // Datensaetze sortieren nach Nachnamen
   quicksort(0, cntData-1, 0, 1);

	// Hat jemand Geburtstag ?
	ourTime = rtcGetTime();

   char timeSt[] = "xx";

   for(i = 0; i < cntData; i++)
   {
   	stringPutHexA(timeSt, ourTime->tm_mon,  2);

		// Stimme der Tag ?
      if(strncmp(AllData[i].strBirthday, timeSt, 2) == 0)
		{
      	stringPutHexA(timeSt, ourTime->tm_year, 2);

			// Stimmt der Monat ?
			if(strncmp(AllData[i].strBirthday+3, timeSt, 2) == 0)
			{
   		   // gefunden
            int a = ShowMessage(6);
				char strName[30];
				snprintf(strName, 30, "%s %s", AllData[i].strVorname, AllData[i].strNachname);
            graphicsStringA(&screenVideo, 5, 25, &spriteFont6x10, std6x10_, 5, 0, strName);

            graphicsStringA(&screenVideo, 5, 45, &spriteFont6x10, std6x10_, 5, 0, AllData[i].strBirthday);
            graphicsStringA(&screenVideo, 100, 45, &spriteFont6x10, std6x10_, 5, 0, AllData[i].strFestnetznr);

				int key = 0;
				for(;;)
				{
					key = buttonsGetStatusA();          // CHECK buttons
					if (key & BUTTONS_AV300_ANY)
					{
                  osdSetComponentConfigA(OSD_VIDEO1, 0);
                  delay(0x15000);
						break;
					}
				}
			}
		}
	}
}

void DisplayDate()
{
	ourTime = rtcGetTime();

// Folgende Variablen sind falsch gef?llt:
//
// tm_yday zeigt nicht die x.ten Tag des Jahres an
// tm_year zeigt den Monat an
// tm_mon zeigt den Tag an
// tm_mday zeigt immer 86 an
// tm_wday ist die Jahreszahl (mu? 2000 addiert werden)

   char timeSt[] = "xx.xx.20xx";
	stringPutHexA(timeSt+8, ourTime->tm_wday, 2);
	stringPutHexA(timeSt+3, ourTime->tm_year, 2);
	stringPutHexA(timeSt,   ourTime->tm_mon,  2);

   pal16[1] = ADDRESS_TEXTCOLOR_HEAD;
   graphicsStringA(&screenBitmap, 578, 5, &spriteFont6x10, std6x10_, 6, 0, timeSt);
}

int get_CntData()
{
   return cntData;
}

//----routine to provide delay-----
void delay(unsigned int time) {
    for (;time>0;time--) ;
}

void ClearScreen()
{
    // Clear full Screen with black color
   graphicsBoxfA(&screenBitmap, 0, 0, 640, 240, ADDRESS_BACKCOLOR_SEPERATOR);
}

int Random(int min, int max)
{
   int r = -1;

   while(r <= min)
   {
      r = rand() % max;
   }

   return r;
}

int GetTime()
{
   int seconds = 0;

   ourTime = rtcGetTime();

   seconds = ourTime->tm_hour*3600;
   seconds += ourTime->tm_min*60;
   seconds += ourTime->tm_sec;

   return seconds;
}

void ShowBuffer()
{
   char buffer[10];
   int i = 0, j = 0;
   int length = 0;
   int row = 3;

//   length = getWordLength(textFile);
   for(j = 0; j < length; j++)
   {
      buffer[0] = *(textFile + j);
      buffer[1] = 0;
      graphicsStringA(&screenBitmap, 1 + j*6, row*10, &spriteFont6x10, std6x10_, 5, 0, buffer);
   }
}

void UpdateElement(int x, int y, int record, int element, int cntChars)
{
   char buffer[MAX_ELEMENT_CHARS];
   char str[10];

   if(element == 0)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strNachname,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strNachname);
   }
   else if(element == 1)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strVorname,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strVorname);
   }
   else if(element == 2)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strStrasse,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strStrasse);
   }
   else if(element == 3)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strPlz,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strPlz);
   }
   else if(element == 4)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strLand,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strLand);
   }
   else if(element == 5)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strWohnort,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strWohnort);
   }
   else if(element == 6)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strEmail,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strEmail);
   }
   else if(element == 7)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strFestnetznr,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strFestnetznr);
   }
   else if(element == 8)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strHandynr,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strHandynr);
   }
   else if(element == 9)
   {
      if(cntChars > 0)
      {
         strncpy(buffer, AllData[record].strBirthday,cntChars);
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, buffer);
      }
      else
         graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 6, 0, AllData[record].strBirthday);
   }
}

void printNr(char* buffer, int nr, int withSign)
{
   int begin = 0;

   if(withSign == 1)
   {
      if(nr < 0)
      {
         nr=nr*(-1);
         buffer[begin++] = '-';
      }
      else
      {
         buffer[begin++] = '+';
      }
   }

   buffer[begin++] = '0' + (nr / 10000) % 10;
   buffer[begin++] = '0' + (nr / 1000) % 10;
   buffer[begin++] = '0' + (nr / 100) % 10;
   buffer[begin++] = '0' + (nr / 10) % 10;
   buffer[begin++] = '0' + (nr / 1) % 10;
   buffer[begin++] = 0;
}

// ShowMode = 0 --> Nach Nachnamen sortierte normale Ansicht
// ShowMode = 1 --> Geburtstagsliste nach Geburtsdatum sortiert
void ShowRange(int begin, int end, int ShowMode)
{
   int i = 0;
   int yPos = 30;
   int xPos = 10;

   pal16[1] = ADDRESS_TEXTCOLOR_LIST;

   if(begin < 0)
   {
      begin = 0;
      end = MAX_ENTRIES_PAGE-1;
   }

   if(begin >= get_CntData()) return; // Abbruch

   if(end > get_CntData()) end = get_CntData();

   graphicsBoxfA(&screenBitmap, 0, 23, 275, 297, ADDRESS_BACKCOLOR_LIST);

   for( i = begin; i < end; i++)
   {
	   if(ShowMode == 0)
		{
			UpdateElement(xPos, yPos, i, 0, 15);
			xPos +=100;
			UpdateElement(xPos, yPos, i, 1, 10);
			xPos +=70;
			UpdateElement(xPos, yPos, i, 7, 15);
      }
		else
		{
			UpdateElement(xPos, yPos, i, 9, 10);
			xPos +=70;
			UpdateElement(xPos, yPos, i, 0, 18);
			xPos +=120;
			UpdateElement(xPos, yPos, i, 1, 12);
		}

      yPos+=12;
      xPos = 10;
   }
}

void ShowStatus()
{
   pal16[1] = ADDRESS_TEXTCOLOR_STATUS;  //0xfb is yellow in the built-in 256-color pallette

   // unterer Rand
   graphicsBoxfA(&screenBitmap, 0, 220, 273, 20, ADDRESS_BACKCOLOR_STATUS); // mittleres Blau als Statuszeile unten
   char str[10];
   printNr(str, cntData, 0);
   graphicsStringA(&screenBitmap, 10, 225, &spriteFont6x10, std6x10_, 5, 0, str);
   graphicsStringA(&screenBitmap, 40, 225, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_RECORDS);

   printNr(str, RangeBegin+1, 0);
   graphicsStringA(&screenBitmap, 115, 225, &spriteFont6x10, std6x10_, 5, 0, str);
   graphicsStringA(&screenBitmap, 145, 225, &spriteFont6x10, std6x10_, 5, 0, "-");
   printNr(str, RangeEnd, 0);
   graphicsStringA(&screenBitmap, 155, 225, &spriteFont6x10, std6x10_, 5, 0, str);
}

void ShowEntryStatus()
{
   pal16[1] = ADDRESS_TEXTCOLOR_STATUS;  //0xfb is yellow in the built-in 256-color pallette

   // unterer Rand
   graphicsBoxfA(&screenBitmap, 0, 220, 273, 20, ADDRESS_BACKCOLOR_STATUS); // mittleres Blau als Statuszeile unten
   char str[10];
   printNr(str, RangeBegin+CurrentEntry+1, 0);
   graphicsStringA(&screenBitmap, 10, 225, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_RECORD);
   graphicsStringA(&screenBitmap, 50, 225, &spriteFont6x10, std6x10_, 5, 0, str);
   graphicsStringA(&screenBitmap, 81, 225, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_OF);
   printNr(str, cntData, 0);
   graphicsStringA(&screenBitmap, 100, 225, &spriteFont6x10, std6x10_, 5, 0, str);
}

void PlaceRowMarker()
{
   if(get_CntData() > 0)
   {
       osdSetComponentConfigA(OSD_BITMAP2, OSD_COMPONENT_ENABLE
                                         | OSD_BITMAP_8BIT);

       osdSetComponentPositionA(OSD_BITMAP2, 0x14, 0x12 + 30 + 1 + (CurrentEntry*12));
   }
   else
   {
       osdSetComponentConfigA(OSD_BITMAP2, 0);
   }
}

void quicksort(int lo, int hi, int element, int asc)
{
   int i = lo;
   int j = hi;

   AddressInfo x = AllData[(lo+hi)/2];

   while(i<=j)
   {
      if(element == 0)
      {
         while( strcmp(AllData[i].strNachname, x.strNachname) < 0) i++;
         while( strcmp(AllData[j].strNachname, x.strNachname) > 0) j--;
      }
      else if(element == 1)
      {
         while( strcmp(AllData[i].strVorname, x.strVorname) < 0) i++;
         while( strcmp(AllData[j].strVorname, x.strVorname) > 0) j--;
      }
      else if(element == 2)
      {
         while( strcmp(AllData[i].strStrasse, x.strStrasse) < 0) i++;
         while( strcmp(AllData[j].strStrasse, x.strStrasse) > 0) j--;
      }
      else if(element == 3)
      {
         while( strcmp(AllData[i].strPlz, x.strPlz) < 0) i++;
         while( strcmp(AllData[j].strPlz, x.strPlz) > 0) j--;
      }
      else if(element == 4)
      {
         while( strcmp(AllData[i].strLand, x.strLand) < 0) i++;
         while( strcmp(AllData[j].strLand, x.strLand) > 0) j--;
      }
      else if(element == 5)
      {
         while( strcmp(AllData[i].strWohnort, x.strWohnort) < 0) i++;
         while( strcmp(AllData[j].strWohnort, x.strWohnort) > 0) j--;
      }
      else if(element == 6)
      {
         while( strcmp(AllData[i].strEmail, x.strEmail) < 0) i++;
         while( strcmp(AllData[j].strEmail, x.strEmail) > 0) j--;
      }
      else if(element == 7)
      {
         while( strcmp(AllData[i].strFestnetznr, x.strFestnetznr) < 0) i++;
         while( strcmp(AllData[j].strFestnetznr, x.strFestnetznr) > 0) j--;
      }
      else if(element == 8)
      {
         while( strcmp(AllData[i].strHandynr, x.strHandynr) < 0) i++;
         while( strcmp(AllData[j].strHandynr, x.strHandynr) > 0) j--;
      }
      else if(element == 9)
      {
		   char strDataAll[MAX_BIRTHDAY];
		   char strDataX[MAX_BIRTHDAY];
         memset(&strDataAll, 0, sizeof(strDataAll));
         memset(&strDataX, 0, sizeof(strDataX));

			if(strlen(x.strBirthday) > 4)
			{
				strncpy(strDataX, x.strBirthday+3, 2);
				strncpy(strDataX+2, x.strBirthday, 2);
			}
			else
				strcpy(strDataX, x.strBirthday);


			if(strlen(AllData[i].strBirthday) > 4)
			{
				strncpy(strDataAll, AllData[i].strBirthday+3, 2);
				strncpy(strDataAll+2, AllData[i].strBirthday, 2);
         }
			else
				strcpy(strDataAll, AllData[i].strBirthday);


         while( strcmp(strDataAll, strDataX) < 0)
			{
			   i++;

				if(strlen(AllData[i].strBirthday) > 4)
				{
					strncpy(strDataAll, AllData[i].strBirthday+3, 2);
					strncpy(strDataAll+2, AllData[i].strBirthday, 2);
				}
				else
					strcpy(strDataAll, AllData[i].strBirthday);
			}

         memset(&strDataAll, 0, sizeof(strDataAll));

			if(strlen(AllData[j].strBirthday) > 4)
			{
				strncpy(strDataAll, AllData[j].strBirthday+3, 2);
   			strncpy(strDataAll+2, AllData[j].strBirthday, 2);
			}
			else
				strcpy(strDataAll, AllData[j].strBirthday);

         while( strcmp(strDataAll, strDataX) > 0)
         {
			   j--;

				memset(&strDataAll, 0, sizeof(strDataAll));

				if(strlen(AllData[j].strBirthday) > 4)
				{
					strncpy(strDataAll, AllData[j].strBirthday+3, 2);
					strncpy(strDataAll+2, AllData[j].strBirthday, 2);
				}
				else
					strcpy(strDataAll, AllData[j].strBirthday);
			}
      }


      // Aufsteigend
      if(i<=j)
      {
         exchange(i,j);
         i++;
	 j--;
      }
   }

   // Rekursion
   if(lo<j) quicksort(lo,j, element, asc);
   if(i<hi) quicksort(i,hi, element, asc);
}

void exchange(int i, int j)
{
   AddressInfo t = AllData[i];
   AllData[i] = AllData[j];
   AllData[j] = t;
}

int SetUnderlinedCursor(int x, int y, int ascii)
{
   char temp[2];
   temp[0] = ascii;
   temp[1] = '\0';

   graphicsBoxfA(&screenBitmap, x, y, 6, 11, ADDRESS_BACKCOLOR_ELEMENTENTRY);
   graphicsStringA(&screenBitmap, x, y, &spriteFont6x10, std6x10_, 5, 0, temp);
   graphicsBoxfA(&screenBitmap, x, y+10, 6, 1, ADDRESS_BACKCOLOR_UNDERLINER);

   return ascii;
}

int GetCurrentAscii(AddressInfo tmpAddress, int currentElement, int index, int oldascii)
{
   int ascii = ASCII_DEFAULT; // A

   if(currentElement == 0)
	{
	   if(index >= strlen(tmpAddress.strNachname))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strNachname[index];
	}
   else if(currentElement == 1)
	{
	   if(index >= strlen(tmpAddress.strVorname))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strVorname[index];
	}
   else if(currentElement == 2)
	{
	   if(index >= strlen(tmpAddress.strStrasse))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strStrasse[index];
	}
   else if(currentElement == 3)
	{
	   if(index >= strlen(tmpAddress.strPlz))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strPlz[index];
	}
   else if(currentElement == 4)
	{
	   if(index >= strlen(tmpAddress.strLand))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strLand[index];
	}
   else if(currentElement == 5)
	{
	   if(index >= strlen(tmpAddress.strWohnort))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strWohnort[index];
	}
   else if(currentElement == 6)
	{
	   if(index >= strlen(tmpAddress.strEmail))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strEmail[index];
	}
   else if(currentElement == 7)
	{
	   if(index >= strlen(tmpAddress.strFestnetznr))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strFestnetznr[index];
	}
   else if(currentElement == 8)
	{
	   if(index >= strlen(tmpAddress.strHandynr))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strHandynr[index];
	}
   else if(currentElement == 9)
	{
	   if(index >= strlen(tmpAddress.strBirthday))
		   ascii = oldascii;
		else
         ascii = tmpAddress.strBirthday[index];
	}

   if(ascii < ASCII_SET_BEGIN)
      ascii = ASCII_DEFAULT;

   return ascii;
}

void ShowAscii(int ascii)
{
   char str[10];
   printNr(str, ascii, 0);
   graphicsBoxfA(&screenBitmap, 5, 220, 100, 240, ADDRESS_BACKCOLOR_ELEMENTENTRY); // weisser Hintergrund
   graphicsStringA(&screenBitmap, 10, 225, &spriteFont6x10, std6x10_, 5, 0, str);
}

int ShowMessage(int type)
{
   osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE | OSD_BITMAP_8BIT);
   osdSetComponentPositionA(OSD_VIDEO1, 0x14+90, 0x12 + 90 );
   pal32[1] = ADDRESS_TEXTCOLOR_MESSAGE;
   graphicsBoxfA(&screenVideo, 0, 0, 384, 60, ADDRESS_BACKCOLOR_MESSAGE);

   if(type == 0)
   {
      // Speichermessage
      graphicsStringA(&screenVideo, 50, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_SAVE);
      graphicsBoxfA(&screenVideo, 0, 17, 384, 2, ADDRESS_BACKCOLOR_MESSAGE_SEP);
   }
   else if((type == 1) || (type == 5) )
   {
      // Loeschmessage
      RedBorderForMessage();

      graphicsStringA(&screenVideo, 10, 10, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_DELETE1);
      graphicsStringA(&screenVideo, 10, 20, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_DELETE2);
      pal16[1] = ADDRESS_TEXTCOLOR_MENU;

		if(type == 5)
         graphicsBoxfA(&screenBitmap, 595, 145, 44, 298, ADDRESS_BACKCOLOR_MENU); // dunkelblau
		else
         graphicsBoxfA(&screenBitmap, 595, 22, 44, 298, ADDRESS_BACKCOLOR_MENU); // dunkelblau, rechter Rand

      graphicsStringA(&screenBitmap, 600,150, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_DELETE);
      graphicsStringA(&screenBitmap, 600,180, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_CANCEL);

		int key = 0;
		for(;;)
		{
			key = buttonsGetStatusA();          // CHECK buttons

			if (key & BUTTONS_AV300_MENU2)
			{ // Cancel
            osdSetComponentConfigA(OSD_VIDEO1, 0);
            delay(0x15000);
			   return RET_CANCEL;
			}

			if (key & BUTTONS_AV300_MENU1)
			{  //F2 button - Delete
            osdSetComponentConfigA(OSD_VIDEO1, 0);
            delay(0x15000);
			   return RET_OK;
			}
		}
   }
   else if(type == 2)
   {
      // Not saved-Message
      RedBorderForMessage();

      graphicsStringA(&screenVideo, 5, 10, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_CONTINUE);
      graphicsStringA(&screenVideo, 5, 30, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_CHANGESLOST);
      graphicsBoxfA(&screenBitmap, 595, 145, 44, 298, ADDRESS_BACKCOLOR_MENU);
      pal16[1] = ADDRESS_TEXTCOLOR_MENU;
      graphicsStringA(&screenBitmap, 600,150, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_OK);
      graphicsStringA(&screenBitmap, 600,180, &spriteFont6x10, std6x10_, 5, 0, ADDRESS_TEXT_CANCEL);

		int key = 0;
		for(;;)
		{
			key = buttonsGetStatusA();          // CHECK buttons

			if (key & BUTTONS_AV300_MENU2)
			{ // Cancel
            osdSetComponentConfigA(OSD_VIDEO1, 0);
            delay(0x15000);
			   return RET_CANCEL;
			}

			if (key & BUTTONS_AV300_MENU1)
			{  //F2 button - ok
            osdSetComponentConfigA(OSD_VIDEO1, 0);
            delay(0x15000);
			   return RET_OK;
			}
		}
   }
   else if(type == 4)
   {
      // Speichermessage
      graphicsStringA(&screenVideo, 10, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_LOADERROR);
      graphicsBoxfA(&screenVideo, 0, 17, 384, 2, ADDRESS_BACKCOLOR_MESSAGE_SEP);
   }
   else if(type == 6)
   {
      // Geburtstag
      graphicsStringA(&screenVideo, 5, 5, &spriteFont6x10, std6x10_, 6, 0, ADDRESS_TEXT_MSG_BIRTHDAY_REMINDER);
      graphicsBoxfA(&screenVideo, 0, 17, 384, 2, ADDRESS_BACKCOLOR_MESSAGE_SEP);
   }
}

void RedBorderForMessage()
{
	graphicsBoxfA(&screenVideo, 0, 0, 384, 2,  ADDRESS_BACKCOLOR_MESSAGE_BORDER);//oben
	graphicsBoxfA(&screenVideo, 382, 2, 2, 58, ADDRESS_BACKCOLOR_MESSAGE_BORDER);//rechts
	graphicsBoxfA(&screenVideo, 0, 2, 2, 58,   ADDRESS_BACKCOLOR_MESSAGE_BORDER);//links
	graphicsBoxfA(&screenVideo, 0, 58, 384, 2, ADDRESS_BACKCOLOR_MESSAGE_BORDER);//unten
}

AddressInfo CopyDataEntry(AddressInfo ToCopy)
{
   AddressInfo tmpAddress;
   memset(&tmpAddress, 0, sizeof(AddressInfo));

   strcpy(tmpAddress.strNachname,   ToCopy.strNachname);
	strcpy(tmpAddress.strVorname,    ToCopy.strVorname);
	strcpy(tmpAddress.strStrasse,    ToCopy.strStrasse);
	strcpy(tmpAddress.strPlz,        ToCopy.strPlz);
	strcpy(tmpAddress.strLand,       ToCopy.strLand);
	strcpy(tmpAddress.strWohnort,    ToCopy.strWohnort);
	strcpy(tmpAddress.strEmail,      ToCopy.strEmail);
	strcpy(tmpAddress.strFestnetznr, ToCopy.strFestnetznr);
	strcpy(tmpAddress.strHandynr,    ToCopy.strHandynr);
   strcpy(tmpAddress.strBirthday,   ToCopy.strBirthday);

	return tmpAddress;
}

void DeleteChar(char* src, char* dest, int index)
{
   int cnt = 0;
	int dstindex = 0;

	for( cnt = 0; cnt < strlen(src); cnt++)
	{
	   if(cnt != index)
		{
	      dest[dstindex] = src[cnt];
			dstindex++;
		}
	}

	dest[dstindex] = '\0';
}

// Bereiche in folgender Reihenfolge:
// 33-44 Sonderzeichen
// 58-63 Sonderzeichen
// 91-96 Sonderzeichen
// 123-126 Sonderzeichen
// 45-57 Zahlen
// 32 Space
// 64-90 Grossbuchstaben
// 97-122 Kleinbuchstaben
// 192-255 Erweiterter Zeichensatz
int NextAscii(int lastAscii)
{
   int ascii = ASCII_DEFAULT;

   if(lastAscii == 32) // Space
      ascii = 64; // auf @ und Grossbuchstaben springen
	else if(lastAscii == 90)
      ascii = 97;
	else if(lastAscii == 122)
	   ascii = 192;
	else if(lastAscii == 255)
      ascii = 33;
	else if(lastAscii == 44)
	   ascii = 58;
	else if(lastAscii == 63)
      ascii = 91;
	else if(lastAscii == 96)
      ascii = 123;
	else if(lastAscii == 126)
      ascii = 45;
	else if(lastAscii == 57)
      ascii = 32;
	else
      ascii = lastAscii+1;
/*
   char str[10];
   printNr(str, ascii, 0);
   pal16[1] = 0x0000;
	graphicsBoxfA(&screenBitmap, 195, 5, 215, 15,  0xFFFF);
   graphicsStringA(&screenBitmap, 200, 10, &spriteFont6x10, std6x10_, 6, 0, str);
*/
	return ascii;
}

int PrevAscii(int lastAscii)
{
   int ascii = ASCII_DEFAULT;

   if(lastAscii == 33)
	   ascii = 255;
	else if(lastAscii == 192)
      ascii = 122;
	else if(lastAscii == 97)
	   ascii = 90;
	else if(lastAscii == 64)
      ascii = 32;
	else if(lastAscii == 32)
      ascii = 57;
	else if(lastAscii == 45)
      ascii = 126;
	else if(lastAscii == 123)
      ascii = 96;
	else if(lastAscii == 91)
      ascii = 63;
	else if(lastAscii == 58)
      ascii = 44;
	else
      ascii = lastAscii-1;

	return ascii;
}

int colorRGB2Packed(int r, int g, int b)
{
	int y = 299*r + 587*g + 114*b;
	int cb = -169*r + -331*g + 500*b + 128000;
	int cr = 500*r - 419*g - 81*b + 128000;

	y = (y+500)/1000;
	cb = (cb+500)/1000;
	cr = (cr+500)/1000;
	if (y < 0) y = 0;
	if (y > 255) y = 255;
	if (cb < 0) cb = 0;
	if (cb > 255) cb = 255;
	if (cr < 0) cr = 0;
	if (cr > 255) cr = 255;

return 0x80000000 | cr << 16 | y << 8 | cb;
}

void ReadIniFile()
{
   char str[10];
	char strIniData[MAX_INI_ROW_LENGTH];
   int cntElement = 0;
   char* pbuffer = textFile;
   char *p2;
   int ret = 0;
	int cnt = 0;
	int cntChars = 0;
	int y = 0;
	int hex = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	bool rFound = false;
	bool gFound = false;
	bool fComment = false;

   memset(&strIniData, 0, sizeof(strIniData));

   ret = loadTextFile("/ABOOK/abook.ini");

   if(ret == 0) return;

   while(*pbuffer != 0)
   {
	   cntChars = 0;
      p2 = pbuffer;
		fComment = false;
      memset(&strIniData, 0, sizeof(strIniData));

      while(*p2 != '\n' && *p2 != 0)  // Trennzeichen suchen
      {
         p2++;
			cntChars++;
      }

      if(*p2 == 0)
         break;

		if(cntChars > 0)
		{
		   if(cntChars > MAX_INI_ROW_LENGTH-1)
			   cntChars = MAX_INI_ROW_LENGTH-1;

			strncpy(strIniData, pbuffer, cntChars);
			strIniData[cntChars] = '\0';

			if((strIniData[0] == '/') || (strIniData[0] == ' ') ) // Comment
			{
				fComment = true;
			}

			if(fComment == false) // new entry
			{
				cnt++;

				if(cnt == 1)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_TEXTCOLOR_HEAD   = hex;
				}
				else if(cnt == 2)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_TEXTCOLOR_LIST   = hex;
				}
				else if(cnt == 3)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_TEXTCOLOR_STATUS = hex;
					pal16[0] = hex;
				}
				else if(cnt == 4)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_TEXTCOLOR_MENU   = hex;
				}
				else if(cnt == 5)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_TEXTCOLOR_ELEMENTNAME = hex;
				}
				else if(cnt == 6)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_TEXTCOLOR_ELEMENTENTRY = hex;
				}
				else if(cnt == 7)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_HEAD = hex;
				}
				else if(cnt == 8)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_LIST = hex;
				}
				else if(cnt == 9)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_STATUS = hex;
				}
				else if(cnt == 10)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_MENU = hex;
				}
				else if(cnt == 11)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_SEPERATOR = hex;
				}
				else if(cnt == 12)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_ELEMENTNAME = hex;
				}
				else if(cnt == 13)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_ELEMENTENTRY = hex;
				}
				else if(cnt == 14)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_MARKER = hex;
				}
				else if(cnt == 15)
				{
					sscanf(strIniData, "%x", &hex);
					ADDRESS_BACKCOLOR_UNDERLINER = hex;
				}
				// Ab hier Videofarben (r,g,b)
				else if(cnt == 16) // r
				{
					r = atoi(strIniData);
					rFound = true;
				}
				else if(cnt == 17) // g
				{
					g = atoi(strIniData);
					gFound = true;
				}
				else if(cnt == 18) // b
				{
					b = atoi(strIniData);

					if(rFound && gFound)
					{
						ADDRESS_TEXTCOLOR_MESSAGE = colorRGB2Packed(r, g, b);
					}
					rFound = false; gFound = false;
					r = 0; g = 0; b = 0;
				}
				else if(cnt == 19) // r
				{
					r = atoi(strIniData); rFound = true;
				}
				else if(cnt == 20) // g
				{
					g = atoi(strIniData); gFound = true;
				}
				else if(cnt == 21) // b
				{
					b = atoi(strIniData);

					if(rFound && gFound)
					{
						ADDRESS_BACKCOLOR_MESSAGE = colorRGB2Packed(r, g, b);
					}
					rFound = false; gFound = false;
					r = 0; g = 0; b = 0;
				}
				else if(cnt == 22) // r
				{
					r = atoi(strIniData); rFound = true;
				}
				else if(cnt == 23) // g
				{
					g = atoi(strIniData); gFound = true;
				}
				else if(cnt == 24) // b
				{
					b = atoi(strIniData);

					if(rFound && gFound)
					{
						ADDRESS_BACKCOLOR_MESSAGE_BORDER = colorRGB2Packed(r, g, b);
					}
					rFound = false; gFound = false;
					r = 0; g = 0; b = 0;
				}
				else if(cnt == 25) // r
				{
					r = atoi(strIniData); rFound = true;
				}
				else if(cnt == 26) // g
				{
					g = atoi(strIniData); gFound = true;
				}
				else if(cnt == 27) // b
				{
					b = atoi(strIniData);

					if(rFound && gFound)
					{
						ADDRESS_BACKCOLOR_MESSAGE_SEP = colorRGB2Packed(r, g, b);
					}
					rFound = false; gFound = false;
					r = 0; g = 0; b = 0;
				}
				/////////////////////
				// TEXTS
				/////////////////////
				else if(cnt == 28)
    				strncpy(ADDRESS_TEXT_NAME, strIniData, sizeof(ADDRESS_TEXT_NAME));
				else if(cnt == 29)
    				strncpy(ADDRESS_TEXT_FIRSTNAME, strIniData, sizeof(ADDRESS_TEXT_FIRSTNAME));
				else if(cnt == 30)
    				strncpy(ADDRESS_TEXT_STREET, strIniData, sizeof(ADDRESS_TEXT_STREET));
				else if(cnt == 31)
    				strncpy(ADDRESS_TEXT_POSTALCODE, strIniData, sizeof(ADDRESS_TEXT_POSTALCODE));
				else if(cnt == 32)
    				strncpy(ADDRESS_TEXT_RESIDENCE, strIniData, sizeof(ADDRESS_TEXT_RESIDENCE));
				else if(cnt == 33)
    				strncpy(ADDRESS_TEXT_COUNTRY, strIniData, sizeof(ADDRESS_TEXT_COUNTRY));
				else if(cnt == 34)
    				strncpy(ADDRESS_TEXT_PHONE, strIniData, sizeof(ADDRESS_TEXT_PHONE));
				else if(cnt == 35)
    				strncpy(ADDRESS_TEXT_HANDY, strIniData, sizeof(ADDRESS_TEXT_HANDY));
				else if(cnt == 36)
    				strncpy(ADDRESS_TEXT_MAIL, strIniData, sizeof(ADDRESS_TEXT_MAIL));
				else if(cnt == 37)
    				strncpy(ADDRESS_TEXT_BIRTHDAY, strIniData, sizeof(ADDRESS_TEXT_BIRTHDAY));
				else if(cnt == 38)
    				strncpy(ADDRESS_TEXT_QUIT, strIniData, sizeof(ADDRESS_TEXT_QUIT));
				else if(cnt == 39)
    				strncpy(ADDRESS_TEXT_NEW, strIniData, sizeof(ADDRESS_TEXT_NEW));
				else if(cnt == 40)
    				strncpy(ADDRESS_TEXT_DELETE, strIniData, sizeof(ADDRESS_TEXT_DELETE));
				else if(cnt == 41)
    				strncpy(ADDRESS_TEXT_SHOW, strIniData, sizeof(ADDRESS_TEXT_SHOW));
				else if(cnt == 42)
    				strncpy(ADDRESS_TEXT_OK, strIniData, sizeof(ADDRESS_TEXT_OK));
				else if(cnt == 43)
    				strncpy(ADDRESS_TEXT_EDIT, strIniData, sizeof(ADDRESS_TEXT_EDIT));
				else if(cnt == 44)
    				strncpy(ADDRESS_TEXT_CANCEL, strIniData, sizeof(ADDRESS_TEXT_CANCEL));
				else if(cnt == 45)
    				strncpy(ADDRESS_TEXT_NEXT, strIniData, sizeof(ADDRESS_TEXT_NEXT));
				else if(cnt == 46)
    				strncpy(ADDRESS_TEXT_RECORDS, strIniData, sizeof(ADDRESS_TEXT_RECORDS));
				else if(cnt == 47)
    				strncpy(ADDRESS_TEXT_RECORD, strIniData, sizeof(ADDRESS_TEXT_RECORD));
				else if(cnt == 48)
    				strncpy(ADDRESS_TEXT_OF, strIniData, sizeof(ADDRESS_TEXT_OF));
				else if(cnt == 49)
    				strncpy(ADDRESS_TEXT_SHOW_ENTRY, strIniData, sizeof(ADDRESS_TEXT_SHOW_ENTRY));
				else if(cnt == 50)
    				strncpy(ADDRESS_TEXT_EDIT_ENTRY, strIniData, sizeof(ADDRESS_TEXT_EDIT_ENTRY));
				else if(cnt == 51)
    				strncpy(ADDRESS_TEXT_NEW_ENTRY, strIniData, sizeof(ADDRESS_TEXT_NEW_ENTRY));
				else if(cnt == 52)
    				strncpy(ADDRESS_TEXT_MSG_CONTINUE, strIniData, sizeof(ADDRESS_TEXT_MSG_CONTINUE));
				else if(cnt == 53)
    				strncpy(ADDRESS_TEXT_MSG_CHANGESLOST, strIniData, sizeof(ADDRESS_TEXT_MSG_CHANGESLOST));
				else if(cnt == 54)
    				strncpy(ADDRESS_TEXT_MSG_DELETE1, strIniData, sizeof(ADDRESS_TEXT_MSG_DELETE1));
				else if(cnt == 55)
    				strncpy(ADDRESS_TEXT_MSG_DELETE2, strIniData, sizeof(ADDRESS_TEXT_MSG_DELETE2));
				else if(cnt == 56)
    				strncpy(ADDRESS_TEXT_MSG_PRESS_KEY, strIniData, sizeof(ADDRESS_TEXT_MSG_PRESS_KEY));
				else if(cnt == 57)
    				strncpy(ADDRESS_TEXT_MSG_SAVE, strIniData, sizeof(ADDRESS_TEXT_MSG_SAVE));
				else if(cnt == 58)
    				strncpy(ADDRESS_TEXT_MSG_LOADERROR, strIniData, sizeof(ADDRESS_TEXT_MSG_LOADERROR));
				else if(cnt == 59)
    				strncpy(ADDRESS_TEXT_MSG_BIRTHDAY_REMINDER, strIniData, sizeof(ADDRESS_TEXT_MSG_BIRTHDAY_REMINDER));
			}
      }

      pbuffer = p2+1;
   }
}
