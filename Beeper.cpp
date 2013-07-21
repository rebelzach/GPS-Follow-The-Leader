
#ifndef BEEPER_cpp
#define BEEPER_cpp

#include <toneAC.h>
#include "Beeper.h"
#include "Definitions.h"

LazyTimer(BeepTimer);
const int BUZZER_PIN = 10;
const int BUZZER_FREQ = 1900;
const int BUZZER_VOL = 10;
const int SILENT_PIN = 6;

Beeper::Beeper() {
  pinMode(BUZZER_PIN, OUTPUT);
  beepState = OFF;
  useLight = OFF;
  useBuzzer = ON;
  beepLength = 0;
  beepPause = 0;
  beepType = NO_BEEP;
  volume = BUZZER_VOL;
}

void Beeper::processLoop() {
  if (beepType == NO_BEEP)
    return;
  
  if (beepType == BEEP_ACKNOWLEDGE) {
    if (beepState == 0) {
      if (useBuzzer) {
        toneAC(BUZZER_FREQ, volume);
      }
      beepState = 1;
    }
    if (LazyTimerDuration(BeepTimer) > 50 && beepState == 1) {
      if (useBuzzer) {
        noToneAC();
      }
      beepState = 2;
    }
    if (LazyTimerDuration(BeepTimer) > 100 && beepState == 2) {
      if (useBuzzer) {
        toneAC(BUZZER_FREQ, volume);
      }
      beepState = 3;
    }
    if (LazyTimerPastDuration(BeepTimer, 150) && beepState == 3) {
      if (useBuzzer) {
        noToneAC();
      }
      beepType = previousBeepType;
      ResetLazyTimer(BeepTimer);
      beepState = 0;
    }
    return;
  }
  
  if (beepType == BEEP_AFFIRMATIVE) {
    if (!LazyTimerDuration(BeepTimer) < 100 && beepState == 0) {
      if (useBuzzer) {
        toneAC(1300, volume);
      }
      beepState = 1;
    }
    if (LazyTimerPastDuration(BeepTimer, 200) && beepState == 1) {
      if (useBuzzer) {
        toneAC(BUZZER_FREQ, volume);
      }
      beepState = 2;
    }
    if (LazyTimerPastDuration(BeepTimer, 300) && beepState == 2) {
      if (useBuzzer) {
        noToneAC();
      }
      beepType = previousBeepType;
      ResetLazyTimer(BeepTimer);
      beepState = 0;
    }
    return;
  }
  
  if (beepState == ON) {
    if (LazyTimerPastDuration(BeepTimer, beepLength)) {
      if (useLight) {
        digitalWrite(SILENT_PIN, LOW);
      } 
      if (useBuzzer) {
        noToneAC();
      }
      beepState = OFF;
      ResetLazyTimer(BeepTimer);
    }
  } else {
    if (LazyTimerPastDuration(BeepTimer, beepPause)) {
      if (useLight) {
        digitalWrite(SILENT_PIN, HIGH);
      }
      if (useBuzzer) {
          toneAC(BUZZER_FREQ, volume);
      }
      beepState = ON;
      ResetLazyTimer(BeepTimer);
    }
  } 
  
}

void Beeper::beepSlow() {
  debugPrintln("Beep Slow");
  beepPause = 1000;
  beepLength = 200;
  startBeepWithType(BEEP_WITH_RATE);
}

void Beeper::beepAcknowledge()
{
  startBeepWithType(BEEP_ACKNOWLEDGE);
}

void Beeper::beepMedium() {
  beepPause =500;
  beepLength = 100;
  debugPrintln("Beep Med");
  startBeepWithType(BEEP_WITH_RATE);
}

void Beeper::beepFast() {
  beepPause = 180;
  beepLength = 75;
  debugPrintln("Beep Fast");
  startBeepWithType(BEEP_WITH_RATE);
}

void Beeper::beepReallyFast() {
  beepPause = 75;
  beepLength = 30;
  debugPrintln("Beep Real Fast");
  startBeepWithType(BEEP_WITH_RATE);
}

void Beeper::stopBeeping() {
  debugPrintln("No beep");
  StopLazyTimer(BeepTimer);
  beepType = NO_BEEP;
  beepState = 0;
  noToneAC();
  if (useLight) {
    digitalWrite(SILENT_PIN, LOW);
  }
}

void Beeper::beepNegative() {
  debugPrintln("Beep Negative");
  startBeepWithType(BEEP_NEGATIVE);
}

void Beeper::beepAffirmative() {
  debugPrintln("Beep Affirmative");
  startBeepWithType(BEEP_AFFIRMATIVE);
}

void Beeper::startBeepWithType(int type) {
  if(type == beepType)
    return;
  beepState = 0;
  if (beepType == BEEP_WITH_RATE || beepType == NO_BEEP) {
    previousBeepType = beepType;
  }
  beepType = type;
  StartLazyTimer(BeepTimer);
}

void Beeper::beepWithDuration(unsigned long duration) {
  
}

void Beeper::beepWithRate(int rate) {
  debugPrint("Beep:");
  debugPrintln(rate);
  if (rate > 10)
    rate = 10;
  if (rate < 1) {
    rate = 1;
  }
  beepPause = 1000/rate;
  beepLength = 300/rate;
  
  startBeepWithType(BEEP_WITH_RATE);
}


void Beeper::selfTest() {
  for(int freq = 150; freq < 2000; freq += 50) {
    toneAC(freq, 10);
    delay(40);
  }
  noToneAC();
}

#endif

