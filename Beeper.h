

#ifndef BEEPER_h
#define BEEPER_h

#include "Arduino.h"

const int NO_BEEP = 0;
const int BEEP_WITH_RATE = 1;
const int BEEP_AFFIRMATIVE = 2;
const int BEEP_NEGATIVE = 3;
const int BEEP_ACKNOWLEDGE = 4;

class Beeper
{
  public:
    
    // methods
    Beeper();
    void processLoop();
    void stopBeeping();
    void beepWithRate(int rate);
    void beepSlow();
    void beepMedium();
    void beepFast();
    void beepReallyFast();
    void beepAffirmative();
    void beepAcknowledge();
    void beepNegative();
    boolean useBuzzer;
    int volume;
    void beepWithDuration(unsigned long duration);
    void selfTest();
    
  private:
    void startBeepWithType(int type);
    int beepType;
    int previousBeepType;
    boolean useLight;
    
    int beepState;
    unsigned long beepPause;
    unsigned long beepLength;  
};

#endif
