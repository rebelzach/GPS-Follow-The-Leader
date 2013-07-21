

#ifndef Debounce_h
#define Debounce_h
#include "Definitions.h"
#include "Arduino.h"

class Debounce
{

public:
  Debounce(unsigned long interval_millis, int pin );
  boolean checkForStateChange();
  boolean read();

private:
  unsigned long debounceTimer;
  unsigned long interval_millis;
  boolean state;
  int pin;
};

#endif


