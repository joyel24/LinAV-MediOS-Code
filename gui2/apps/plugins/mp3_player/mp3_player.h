
#define MAIN_WIN      1
#define SETTINGS_WIN  2
#define SOUND_WIN     3

/* refresh.c */
void apply_settings(void);
void refreshScreen(int window);
void draw_settings_progressbar(int x, int y, int value);
void draw_soundsettings(void);
void draw_settings(void);

/* side_menu.c */
void draw_main_help_text(void);
void draw_settings_help_text(void);
void draw_soundsettings_help_text(void);

/* evtHandler.c */
void handleMainWin(int evt);
void handleSettingsWin(int evt);
void handleSoundWin(int evt);

/* osc_peak_meter.c */
void drawPeak(void);
void oscillograph(int l, int r);
void peak_meters(int l, int r);
void erase_peak(void);

/* player_actions.c */
void pause_resume(void);


