#include <Arduino.h>

#include "debug_led.hpp"
#include "oled_display.hpp"
#include "playlist.hpp"

// ======================================================================
// Constants and data structures

const int TIFS_PIN = 7;     // Fast seek, combine with TIFWD or FIREV
const int TIFWD_PIN = 6;    // Forward
const int TIREV_PIN = 5;    // Reverse
const int TIREW_PIN = 4;    // Rewind
const int TOLDPT_PIN = 2;   // "Load point" (BOT marker)
const int TOEOT_PIN = 3;    // EOT marker

// ======================================================================
// Global state

// Timer state
unsigned long g_previousTime = 0;

// ======================================================================
// Function prototypes

// Setup
void setupTapeInputPin(int pin);
void setupTapeOutputPin(int pin);

// Main loop
void tickTimers(void);
void syncPins(void);

// Interrupt handlers
void isrBot(void);
void isrEot(void);

// ======================================================================
// Arduino lifecycle functions

void setup() {
  setupTapeInputPin(TIFS_PIN);
  setupTapeInputPin(TIFWD_PIN);
  setupTapeInputPin(TIREV_PIN);
  setupTapeInputPin(TIREW_PIN);

  setupTapeOutputPin(TOLDPT_PIN);
  setupTapeOutputPin(TOEOT_PIN);

  attachInterrupt(digitalPinToInterrupt(TOLDPT_PIN), isrBot, FALLING);
  attachInterrupt(digitalPinToInterrupt(TOEOT_PIN), isrEot, FALLING);
  interrupts();

  Serial.begin(9600);

  initDisplay();
  // Delay so startup screen can be read
  delay(3000);
  selectPlaylist(0);
}

void loop() {
  tickTimers();
  refreshDisplay();
  syncDebugLedState();

    if (!g_playlistEnabled) {
    // Wait for BOT interrupt
    return;
  }

  syncPins();
}

// ======================================================================
// Interrupt handlers

void isrBot(void) {
  // Beginning of tape, restart playlist
  restartPlaylist();
  turnOffDebugLed();
}

void isrEot(void) {
  // End of tape, assert rewind output
  // Next loop through main program will deassert it
  // BOT ISR will handle restarting the program
  digitalWrite(TIFS_PIN, HIGH);
  digitalWrite(TIFWD_PIN, HIGH);
  digitalWrite(TIREV_PIN, HIGH);
  digitalWrite(TIREW_PIN, LOW);
  disablePlaylist(REWINDING);
  strobeDebugLed();
}

// ======================================================================
// Other functions

void setupTapeInputPin(int pin) {
  // Set INPUT_PULLUP first to avoid low pulse when we set it to OUTPUT
  // as this can cause the tape drive to rewind on startup
  pinMode(pin, INPUT_PULLUP);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void setupTapeOutputPin(int pin) {
  pinMode(pin, INPUT_PULLUP);
}

void tickTimers(void) {
  unsigned long currentTime = millis();
  unsigned long delta = currentTime - g_previousTime;
  updateDebugLedTimers(delta);
  updateDisplayTimers(delta);
  updatePlaylistTimers(delta);
  g_previousTime = currentTime;
}

void syncPins(void) {
  TapeState desiredTapeState = g_playlistStep.state;
  digitalWrite(TIFS_PIN, desiredTapeState == FAST_FORWARD || desiredTapeState == FAST_REVERSE ? LOW : HIGH);
  digitalWrite(TIFWD_PIN, desiredTapeState == FORWARD || desiredTapeState == FAST_FORWARD ? LOW : HIGH);
  digitalWrite(TIREV_PIN, desiredTapeState == REVERSE || desiredTapeState == FAST_REVERSE ? LOW : HIGH);
  digitalWrite(TIREW_PIN, HIGH);
}
