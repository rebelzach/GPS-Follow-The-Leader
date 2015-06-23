
#include "Debounce.h"
#include "Arduino.h"
#include "Definitions.h"

Debounce::Debounce(unsigned long interval_millis, int pin)
{
  StopLazyTimer(debounceTimer);
  this->interval_millis = interval_millis;;
  state = digitalRead(pin);
  this->pin = pin;
}

boolean Debounce::checkForStateChange()
{
  boolean newState = digitalRead(pin);
  if (state != newState) {
  	if (LazyTimerPastDuration(debounceTimer, interval_millis)) {
  		StopLazyTimer(debounceTimer);
  		state = newState;
                return YES;
	} else if (!LazyTimerRunning(debounceTimer)) {
          StartLazyTimer(debounceTimer);
        }
  }
  return NO;
}

boolean Debounce::read()
{
  return state;
}
