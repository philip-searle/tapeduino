#include <Arduino.h>
#include "debug_led.hpp"
#include "oled_display.hpp"
#include "playlist.hpp"

const struct PlaylistStep PLAYLIST_START = { STOPPED, 1 }; // Dummy start step (skipped on inital load)
const struct PlaylistStep PLAYLIST_END = { STOPPED, 0 };   // Zero millis marks end of playlist

#define SECS(s) (s * 1000L)
#define MINS(m) SECS(m * 60L)

// Define playlist step arrays
#define PLAYLIST_START(index, name) \
const struct PlaylistStep PLAYLIST_STEPS_##index[] = { \
  PLAYLIST_START,
#define PLAYLIST_STEP(state, millis) { state, millis },
#define PLAYLIST_END() PLAYLIST_END };
#include "playlists.xpp"
#undef PLAYLIST_START
#undef PLAYLIST_STEP
#undef PLAYLIST_END

// Define playlist headers
#define PLAYLIST_START(index, name) { \
  name, \
  sizeof PLAYLIST_STEPS_##index / sizeof PLAYLIST_STEPS_##index[0], \
  PLAYLIST_STEPS_##index \
},
#define PLAYLIST_STEP(...) /* */
#define PLAYLIST_END() /* */
const struct Playlist PLAYLISTS[] = {
#include "playlists.xpp"
};

// Tape playlist state
volatile boolean g_playlistEnabled = true;
const Playlist *g_playlist = &PLAYLISTS[0];
PlaylistStep g_playlistStep = PLAYLIST_END;
unsigned int g_playlistIndex = 0;
unsigned int g_playlistStepIndex = 0;
const unsigned int MAX_PLAYLIST_INDEX = sizeof PLAYLISTS / sizeof PLAYLISTS[0];

void selectPlaylist(int playlistIndex) {
  g_playlistEnabled = true;
  g_playlist = &PLAYLISTS[playlistIndex];
  g_playlistStep = PLAYLIST_END;
  g_playlistIndex = playlistIndex;
  g_playlistStepIndex = 0;
  notifyDisplayPlaylistChanged();
}

void advancePlaylist(void) {
  g_playlistStepIndex += 1;
  if (g_playlist->steps[g_playlistStepIndex].millis <= 0) {
    // Loop when we reach the sentinal
    g_playlistStepIndex = 0;
  }
  g_playlistStep = g_playlist->steps[g_playlistStepIndex];
  pulseDebugLed();
}

void disablePlaylist(TapeState overrideState) {
  g_playlistEnabled = false;
  g_playlistStep.state = overrideState;
}

void updatePlaylistTimers(long delta) {
  if (!g_playlistEnabled) {
    return;
  }
  g_playlistStep.millis -= delta;
  if (g_playlistStep.millis <= 0) {
    advancePlaylist();
  }
}

void restartPlaylist(void) {
  g_playlistIndex = 0;
  g_playlistStep = PLAYLIST_END;
  g_playlistEnabled = true;
}
