
#ifndef BEEPER_cpp
#define BEEPER_cpp

#include <toneAC.h>
#include "Beeper.h"
#include "Definitions.h"
#include "Pitches.h"

LazyTimer(BeepTimer);
const int BUZZER_PIN = 10;
const int BUZZER_FREQ = 800;
const int BUZZER_VOL = 10;
//const int BUZZER_VOL = 10;
const int SILENT_PIN = 6;

Beeper::Beeper() {
  pinMode(BUZZER_PIN, OUTPUT);
  noteIndex = 0;
  useBuzzer = ON;
  beepType = NO_BEEP;
  volume = BUZZER_VOL;
  currentNoteTotal = 0;
  previousNoteTotal = 0;
}

const int arrivalCount = 12;
Note arrival[arrivalCount] = {{NOTE_C5, 4},{NOTE_G4, 8},{NOTE_G4, 8},{NOTE_A4, 4},{NOTE_G4, 4},{0, 4},{NOTE_B4, 8},{NOTE_C5, 4},{0, 1},{0, 1},{0, 1},{0, 1}};
const int acknowledgeCount = 4;
Note acknowledge[acknowledgeCount] = {{2400, 16},{0, 8},{2400, 16},{0, 8}};

void Beeper::processLoop() {
  if (beepType == NO_BEEP) 
    return;
  
  if (noteIndex == 0) {
    note(currentNoteArray[noteIndex].freq);
    ResetLazyTimer(BeepTimer);
    ++noteIndex;
    return;
  }
  
  if (beepType == BEEP_SINGLE) {
    int duration = 1000/currentNoteArray[noteIndex - 1].duration;
    if (LazyTimerDuration(BeepTimer) > duration) {
      if (noteIndex == currentNoteTotal) { // Last note
        note(0);
        debugPrint("Previous Type:");
        debugPrintln(previousBeepType);
        beepType = previousBeepType;
        copyNotesArrayToArray(previousNoteArray, currentNoteArray, previousNoteTotal);
        currentNoteTotal = previousNoteTotal;
        ResetLazyTimer(BeepTimer);
        noteIndex = 0;
        return;
      }
      note(currentNoteArray[noteIndex].freq);
      ++noteIndex;
      ResetLazyTimer(BeepTimer);
    } 
  }
  
  if (beepType == BEEP_REPEATING) {
    if (LazyTimerDuration(BeepTimer) > 1000/currentNoteArray[noteIndex - 1].duration) {
      if (noteIndex == currentNoteTotal) { // Last note
        ResetLazyTimer(BeepTimer);
        noteIndex = 0;
        return;
      }
      note(currentNoteArray[noteIndex].freq);
      ResetLazyTimer(BeepTimer);
      ++noteIndex;
    }
  }
}


void Beeper::beepArrival()
{
  startBeepWithType(BEEP_REPEATING, arrival, arrivalCount);
}

void Beeper::beepAcknowledge()
{
  startBeepWithType(BEEP_SINGLE, acknowledge, acknowledgeCount);
}

void Beeper::beepReallyFast() {
  debugPrintln("Beep Real Fast");
  //startBeepWithType(BEEP_REPEATING);
}

void Beeper::stopBeeping() {
  debugPrintln("No beep");
  StopLazyTimer(BeepTimer);
  beepType = NO_BEEP;
  noteIndex = 0;
  note(0);
}

void Beeper::beepNegative() {
  debugPrintln("Beep Negative");
  //startBeepWithType(BEEP_SINGLE);
}

void Beeper::beepAffirmative() {
  debugPrintln("Beep Affirmative");
  //startBeepWithType(BEEP_SINGLE);
}

void Beeper::startBeepWithType(int type, Note *notes, int noteCount) {
  if(notes == previousNoteArray && type == previousBeepType)
    return;
  debugPrint("Starting Type:");
  debugPrintln(type);
  if (type == BEEP_SINGLE) {
    previousBeepType = beepType;
    debugPrint("Assingned Prev Type:");
  debugPrintln(previousBeepType);
    if (previousBeepType == BEEP_REPEATING) {
      copyNotesArrayToArray(currentNoteArray, previousNoteArray, 2);
      previousNoteTotal = currentNoteTotal;
    }
  }
  
  noteIndex = 0;
  beepType = type;
  copyNotesArrayToArray(notes, currentNoteArray, noteCount);
  currentNoteTotal = noteCount;
  StartLazyTimer(BeepTimer);
}

void Beeper::beepWithRate(int rate) {
  debugPrint("Beep:");
  debugPrintln(rate);
  if (rate > 15)
    rate = 15;
  if (rate < 1) {
    rate = 1;
  }
  int beepPause = rate * 1.5;
  int beepLength = rate * 2;
  Note notes[2] = {{BUZZER_FREQ, beepLength},{0, beepPause}};
  startBeepWithType(BEEP_REPEATING, notes, 2);
}

void Beeper::note(int freq) {
  if (useBuzzer) {
    toneAC(freq, volume);
  }
}

void Beeper::selfTest() {
  for(int freq = 150; freq < 2000; freq += 50) {
    toneAC(freq, 10);
    delay(40);
  }
  noToneAC();
}

void Beeper::copyNotesArrayToArray(Note *array1, Note *array2, int count) {
  for (int i = 0; i < count; i++) {
    array2[i] = array1[i];
  }
}

#endif

