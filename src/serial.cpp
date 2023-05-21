#include <Arduino.h>
#include "playlist.hpp"

// Unique ID for this device
const char*const DEVICE_ID = "81";

// Maximum length command accepted; must be a power-of-two for wrapping
const size_t COMMAND_BUFFER_LEN = 64;

char commandBuffer[64];
int commandHead = 0;
int commandTail = 0;

void processCommand(void);
void commandPause(void);
void commandRestart(void);
void commandSwitchPlaylist(void);

// Called by Arduino framework after each loop() if serial data is available
void serialEvent(void) {
  while (true) {
    int byte = Serial.read();
    if (byte < 0) {
      return;
    }
    Serial.write(byte);
    if (byte == '\r' || byte == '\n') {
      byte = '\0';
    }
    if (byte >= 'a' && byte <= 'z') {
      // ASCII upcase
      byte &= ~'A' ^ 'a';
    }
    commandBuffer[commandHead++] = byte;
    commandHead &= ~COMMAND_BUFFER_LEN;
    if (commandHead == commandTail) {
      Serial.println(F("Command buffer overflow; ignoring entire command"));
      return;
    }
    if (byte == 0 && commandHead != commandTail) {
      processCommand();
      commandHead = commandTail = 0;
    }
  }
}

void processCommand(void) {
  if (commandBuffer[0] == '\0') {
    return;
  }
  if (commandBuffer[0] != DEVICE_ID[0] ||
      commandBuffer[1] != DEVICE_ID[1]) {
    commandBuffer[2] = '\0';
    Serial.print(F("Ignoring command; not our device ID: "));
    Serial.println(commandBuffer);
    return;
  }
  if (commandBuffer[2] != ' ') {
    Serial.println(F("Invalid command format; expected space after device ID"));
    return;
  }
  switch (commandBuffer[3]) {
    case 'P':
      commandPause();
      return;
    case 'R':
      commandRestart();
      return;
    case 'S':
      commandSwitchPlaylist();
      return;
    default:
      Serial.print(F("Invalid command byte; expected one of [PRS], but got: "));
      Serial.println(commandBuffer[3]);
      return;
  }
}

void commandPause(void) {
  Serial.println(F("Pausing playlist"));
  disablePlaylist(STOPPED);
}

void commandRestart(void) {
  Serial.println(F("Restarting playlist"));
  restartPlaylist();
}

void commandSwitchPlaylist(void) {
  unsigned int playlistIndex = commandBuffer[4] - '0';
  if (playlistIndex < 0 || playlistIndex >= MAX_PLAYLIST_INDEX) {
    Serial.print(F("Playlist index out of range: expected 0-"));
    Serial.println(MAX_PLAYLIST_INDEX - 1);
    return;
  }
  Serial.print(F("Switching to playlist: "));
  Serial.println(playlistIndex);
  selectPlaylist(playlistIndex);
}
