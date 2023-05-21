
#ifndef H_OLED_DISPLAY
#define H_OLED_DISPLAY

void initDisplay(void);
void updateDisplayTimers(long delta);
void refreshDisplay(void);
void notifyDisplayPlaylistChanged(void);

#endif
