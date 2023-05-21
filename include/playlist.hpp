
#ifndef H_PLAYLIST
#define H_PLAYLIST

enum TapeState {
  FAST_REVERSE,
  REVERSE,
  STOPPED,
  FORWARD,
  FAST_FORWARD,

  // Not a real state, just a placeholder for when TIREW has been
  // triggered and we're waiting for the BOT marker.
  REWINDING
};

struct PlaylistStep {
  enum TapeState state;
  long millis;
};

struct Playlist {
  const char *const name;
  const long length;
  const PlaylistStep *steps;
};

extern volatile boolean g_playlistEnabled;
const extern Playlist *g_playlist;
extern PlaylistStep g_playlistStep;
extern unsigned int g_playlistIndex;
extern unsigned int g_playlistStepIndex;
extern const unsigned int MAX_PLAYLIST_INDEX;

void selectPlaylist(int playlist);
void disablePlaylist(TapeState overrideState);
void updatePlaylistTimers(long delta);
void restartPlaylist(void);

#endif
