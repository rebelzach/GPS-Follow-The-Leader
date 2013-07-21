

#ifndef BEEPER_h
#define BEEPER_h

#include "Arduino.h"

const int NO_BEEP = 0;
const int BEEP_REPEATING = 1;
const int BEEP_SINGLE = 2;

struct Note {
  int freq;
  int duration;
};

class Beeper
{
  public:
    
    // methods
    Beeper();
    void processLoop();
    void stopBeeping();
    void beepWithRate(int rate);
    void beepArrival();
    void beepReallyFast();
    void beepAffirmative();
    void beepAcknowledge();
    void beepNegative();
    boolean useBuzzer;
    int volume;
    void selfTest();
    
  private:
    void startBeepWithType(int type, Note *notes, int noteCount);
    int beepType;
    int previousBeepType;
    int currentNoteTotal;
    Note currentNoteArray[15];
    int previousNoteTotal;
    Note previousNoteArray[15];
    void copyNotesArrayToArray(Note *array1, Note *array2, int count);
    int noteIndex; 
    void note(int freq);
};

#endif
