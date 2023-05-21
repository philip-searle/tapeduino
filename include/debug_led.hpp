
#ifndef H_DEBUGLED
#define H_DEBUGLED

void turnOffDebugLed(void);
void pulseDebugLed(void);
void strobeDebugLed(void);
void updateDebugLedTimers(long delta);
void syncDebugLedState(void);

#endif
