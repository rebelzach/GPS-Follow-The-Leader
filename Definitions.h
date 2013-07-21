#ifndef DEFINITIONS_h
#define DEFINITIONS_h

#include "Beeper.h"
#include "FrontControls.h"
const unsigned long ULONG_MAX = 4294967295;
extern boolean adminMode;
extern FrontControls frontControls;
extern Beeper beeper;

#define ON HIGH
#define OFF LOW
#define YES HIGH
#define NO LOW

#define DEBUG 1 // Set this to zero when everything is good
#define debugBegin() do { if (DEBUG) Serial.begin(9600); } while (0)
#define debugPrint(x) do { if (DEBUG) Serial.print(x); } while (0)
#define debugPrintln(x) do { if (DEBUG) Serial.println(x); } while (0)
//#define debugPrintMem() do { if (DEBUG) {Serial.print("MemoryFree"); Serial.println(freeMemory()); } } while (0)

#define LazyTimer(timer) static unsigned long timer = ULONG_MAX
#define StartLazyTimer(timer) (timer = millis())
#define LazyTimerRunning(timer) (timer != ULONG_MAX)
#define ResetLazyTimer(timer) (timer = millis())
#define StopLazyTimer(timer) (timer = ULONG_MAX)
#define LazyTimerPastDuration(timer, x) (timer != ULONG_MAX && millis() - timer > x)
#define LazyTimerDuration(timer) (millis() - timer)

#endif
