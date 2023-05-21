#include <Arduino.h>
#include <limits.h>

// Debug LED state:
//   > 0 == turn on, value is millis to stay lit
//   = 0 == turn off
//   < 0 == strobe LED continuously
long g_debugLedMillis = 0;

const long DEBUG_LED_PULSE_MS = 500;

void turnOffDebugLed(void) {
  g_debugLedMillis = 0;
}

void pulseDebugLed(void) {
  g_debugLedMillis = DEBUG_LED_PULSE_MS;
}

void strobeDebugLed(void) {
  g_debugLedMillis = LONG_MIN;
}

void updateDebugLedTimers(long delta) {
  if (g_debugLedMillis > 0) {
    g_debugLedMillis -= delta;
    if (g_debugLedMillis < 0) {
      g_debugLedMillis = 0;
    }
  } else if (g_debugLedMillis < 0) {
    g_debugLedMillis += delta;
    if (g_debugLedMillis >= 0) {
      g_debugLedMillis = LONG_MIN;
    }
  }
}

void syncDebugLedState(void) {
  if (g_debugLedMillis < 0) {
    digitalWrite(LED_BUILTIN, (g_debugLedMillis & 0x40) != 0);
  } else {
    digitalWrite(LED_BUILTIN, g_debugLedMillis != 0);
  }
}
