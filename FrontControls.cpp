
#ifndef FRONTCONTROLS_cpp
#define FRONTCONTROLS_cpp
#include "FrontControls.h"
#include "Definitions.h"
#include "FrontControls.h"
#include "Arduino.h"
#include "Debounce.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, A3, 4, 5, 6, 7, 8);

const int HOLD_DURATION = 8000;
const int UP_SWITCH_PIN = A0;
const int DOWN_SWITCH_PIN = A1;
const int ENTER_SWITCH_PIN = A2;
const int BACKLIGHT_PIN = 9;
Debounce upSwitch(30 , UP_SWITCH_PIN);
Debounce downSwitch(30 , DOWN_SWITCH_PIN);
Debounce enterSwitch(30 , ENTER_SWITCH_PIN);
LazyTimer(BackLightTimer);
const unsigned long BACKLIGHT_TIMEOUT = 900000;


//const int STATUS_LIGHT_PIN = 6;
const int STATUS_LIGHT_PULSE_TIME = 100;

LazyTimer(StatusPulseTimer);
LazyTimer(HoldTimer);

FrontControls::FrontControls()
{
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, LOW);
  haveFix = YES;

  // Switch
  pinMode(UP_SWITCH_PIN, INPUT);
  pinMode(DOWN_SWITCH_PIN, INPUT);
  pinMode(ENTER_SWITCH_PIN, INPUT);
  // Pullups
  digitalWrite(UP_SWITCH_PIN, HIGH);
  digitalWrite(DOWN_SWITCH_PIN, HIGH);
  digitalWrite(ENTER_SWITCH_PIN, HIGH);
  
  isSelfTesting = NO;
}

void FrontControls::begin() {
  lcd.begin(16, 2);
}
void  FrontControls::guideHasFix() {
  if (haveFix) {
    frontControls.activateBacklight();
    haveFix = YES;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Follow The Beeps");
  }
}

boolean FrontControls::upSwitchState() {
  return !digitalRead(UP_SWITCH_PIN);
}

boolean FrontControls::downSwitchState() {
  return !digitalRead(DOWN_SWITCH_PIN);
}

boolean FrontControls::enterSwitchState() {
  return !digitalRead(ENTER_SWITCH_PIN);
}

void FrontControls::processLoop()
{
   if (LazyTimerPastDuration(BackLightTimer, BACKLIGHT_TIMEOUT)) {
    StopLazyTimer(BackLightTimer);
    digitalWrite(BACKLIGHT_PIN, OFF);;
  }
  if (enterSwitch.checkForStateChange()) {
    if (enterSwitch.read()) {
        handleSwitch(ENTER_SWITCH, PUSHED);
    }
  }
  if (upSwitch.checkForStateChange()) {
    if (upSwitch.read()) {
      if (LazyTimerPastDuration(HoldTimer, HOLD_DURATION)) {
        handleSwitch(UP_SWITCH, HELD);
      } else {
        handleSwitch(UP_SWITCH, PUSHED);
      }
      StopLazyTimer(HoldTimer);
      debugPrintln("up");
    } else {
      // Down
      StartLazyTimer(HoldTimer);
    }
  }
  if (downSwitch.checkForStateChange()) {
    if (downSwitch.read()) {
      if (LazyTimerPastDuration(HoldTimer, HOLD_DURATION)) {
        handleSwitch(DOWN_SWITCH, HELD);
      } else {
        handleSwitch(DOWN_SWITCH, PUSHED);
      }
      StopLazyTimer(HoldTimer);
      debugPrintln("down");
    } else {
      // Down
      StartLazyTimer(HoldTimer);
    }
  }
  
  if (LazyTimerPastDuration(StatusPulseTimer, STATUS_LIGHT_PULSE_TIME)) {
    //digitalWrite(STATUS_LIGHT_PIN, !digitalRead(STATUS_LIGHT_PIN));
    ResetLazyTimer(StatusPulseTimer); 
  }
}

void FrontControls::setSwitchCallbacks(void (*upCallback)(),void (*downCallback)(), void (*enterCallback)()) {
  upSwitchCallback = upCallback;
  downSwitchCallback = downCallback;
  enterSwitchCallback = enterCallback;
}

void FrontControls::setSwitchHeldCallbacks(void (*upHeldCallback)(),void (*downHeldCallback)()) {
  upSwitchHeldCallback = upHeldCallback;
  downSwitchHeldCallback = downHeldCallback;
}

void FrontControls::displayNumber(int number) {
  
}

void FrontControls::handleSwitch(Switches theSwitch, SwitchStatus theStatus) {
  if (isSelfTesting) {
    lcd.setCursor(0, 1);
    lcd.print(theSwitch);
  }
  switch (theSwitch) {
    case UP_SWITCH:
      if (theStatus == PUSHED)
        upSwitchCallback();
      else 
        upSwitchHeldCallback();
      break;
    case DOWN_SWITCH:
      if (theStatus == PUSHED)
        downSwitchCallback();
      else 
        downSwitchHeldCallback();
      break;
    case ENTER_SWITCH:
      if (theStatus == PUSHED)
        enterSwitchCallback();
      break;
  }
}

void FrontControls::selfTest() {
  lcd.write(" Press a Button");
  isSelfTesting = YES;
}

void FrontControls::displayWaypointSelection(int waypointIndex, float lat, float lon) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("Destination: ");
  lcd.print(waypointIndex);
  lcd.setCursor(0,1);
  lcd.print(lat, 3);
  lcd.print(",");
  lcd.print(lon, 3);
}

void FrontControls::generalMessage(String message) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message);
}

void FrontControls::guideMessage(String message) {
  guideMessage(message, "");
} 

void FrontControls::guideMessage(String message1, String message2) {
  if (adminMode) {
     return; 
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message1);
  lcd.setCursor(0,1);
  lcd.print(message2);
} 

void FrontControls::guideOffcourse() {
  if (adminMode) {
     return; 
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Follow The Beeps");
}

void FrontControls::guideOncourse() {
  if (adminMode) {
     return; 
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Follow The Beeps");
}

void FrontControls::displayNoFix() {
  if (!haveFix)
    return;
  frontControls.activateBacklight();
  haveFix = NO;
  debugPrintln("No Signal");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("No Signal");
  lcd.setCursor(0,1);
  lcd.write("Find the sky");
}

void FrontControls::displayGPS(float lat, float lon, float distance, unsigned long precision, int updates, float dFromRef, float recentCourse, float destCourse, int beepRate) {
  lcd.clear();
  lcd.setCursor(0,0);
//  lcd.print(lat, 4);
//  lcd.print(",");
//  lcd.print(lon, 4);
  lcd.print("C");
  lcd.print(recentCourse,0);
  lcd.print("DC");
  lcd.print(destCourse,0);
  lcd.print("BR");
  lcd.print(beepRate);
  lcd.setCursor(0,1);
  lcd.print("U");
  lcd.print(updates);
  lcd.print("R");
  lcd.print(dFromRef);
  lcd.print("D");
  lcd.print(distance, 0);
  lcd.print("P");
  lcd.print(precision);
}

void FrontControls::clear() {
   lcd.clear(); 
}

void FrontControls::updateGuideInfo(float currentCourse, float courseToDest, float currentDistance) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("cc:");
  lcd.print(currentCourse, 1);
  lcd.print("cd:");
  lcd.print(courseToDest, 1);
  lcd.setCursor(0,1);
  lcd.print("Dis:");
  lcd.print(currentDistance, 1);
}

void FrontControls::activateBacklight() {
  digitalWrite(BACKLIGHT_PIN, ON);
  StartLazyTimer(BackLightTimer);
}

#endif
 
