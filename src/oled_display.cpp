#include <Arduino.h>
#include <U8g2lib.h>

#include "oled_display.hpp"
#include "playlist.hpp"

// ======================================================================
// OLED display: SCK = A5, SDA = A4
// Can use other pins if we switch away from hardware SPI
// 128x32 display (16x16 symbols, 10x16 alphanumeric chars)
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0);

// Symbols for current TapeState for font u8g2_font_unifont_t_symbols
const char *const TAPE_STATE_SYMBOLS[]= {
  "\u23ea", // <<   FAST_REVERSE
  "\u23f4", // <    REVERSE
  "\u23f8", // ||   STOPPED
  "\u23f5", //  >   FORWARD
  "\u23e9", //  >>  FAST_FORWARD
  "\u23ee"  // |<<  REWINDING
};

// Display scroll position
constexpr int SCROLL_SPEED = 40;
constexpr int SCROLL_DELAY = 3000;
long g_displayScrollDelay = SCROLL_DELAY;
long g_displayScrollMillis = 0;
long g_displayScrollMax = 0;

void initDisplay(void) {
  display.begin();
  display.enableUTF8Print();

  display.clearBuffer();
  display.setFont(u8g2_font_unifont_t_symbols);
  display.drawRFrame(0, 0, 128, 32, 8);
  display.drawUTF8X2(4, 28, " \u260d v1.0");
  display.sendBuffer();
}

void resetScrollPosition(void) {
  g_displayScrollDelay = SCROLL_DELAY;
  g_displayScrollMillis = 0;
}

void notifyDisplayPlaylistChanged(void) {
  g_displayScrollMax = (60 + display.getUTF8Width(g_playlist->name));
  resetScrollPosition();
}

void updateDisplayTimers(long delta) {
  if (g_displayScrollDelay > 0) {
    g_displayScrollDelay -= delta;
    g_displayScrollMillis = 0;
  } else {
    g_displayScrollMillis += delta;
    if (g_displayScrollMillis / SCROLL_SPEED >= g_displayScrollMax) {
      resetScrollPosition();
    }
  }
}

void refreshDisplay(void) {
  display.clearBuffer();

  display.setCursor(-(g_displayScrollMillis / SCROLL_SPEED),16);
  display.print(F("PL:"));
  display.print(u8x8_u8toa(g_playlistIndex,2));
  display.print(' ');
  display.print(g_playlist->name);

  display.setCursor(26,32);
  display.drawUTF8(0,32, TAPE_STATE_SYMBOLS[g_playlistStep.state]);
  if (g_playlistStep.state == REWINDING || !g_playlistEnabled) {
    display.print("--:-- --/--");
  } else {
    display.print(u8x8_u8toa((g_playlistStep.millis / 1000) / 60L, 2));
    display.print(':');
    display.print(u8x8_u8toa((g_playlistStep.millis / 1000) % 60L, 2));

    display.print(' ');
    display.print(u8x8_u16toa(g_playlistStepIndex, 2));
    display.print('/');
    display.print(u8x8_u16toa(g_playlist->length - 2, 2));
  }

  display.sendBuffer();
}
