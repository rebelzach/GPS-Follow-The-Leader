#include <toneAC.h>

#include "Definitions.h"
#include <SoftwareSerial.h>
#include "FrontControls.h"
#include "GPSGuide.h"
#include <TinyGPS.h>
#include <EEPROM.h>
#include "WaypointStore.h"
#include <LiquidCrystal.h>
//Todo
/*
Arrival song, press a button to silence
Timeout power off
Handle position loss audibly
LATER:
Multi point mode?
*/

FrontControls frontControls;
GPSGuide gpsGuide;
WaypointStore waypoints;
Beeper beeper;
boolean adminMode = NO;
int currentMenuIndex = 0;

void setup() { 
#if 1 
  adminMode = YES;
#endif
  debugBegin();
  delay(200);// Settle
  frontControls.begin();
  frontControls.displayNumber(currentMenuIndex);
  if (frontControls.upSwitchState() == ON) {
      adminMode = YES;
  }
  
  if (frontControls.downSwitchState() == ON) {
    delay(5000);
    if (frontControls.downSwitchState() == ON) {
      debugPrintln("Restore Blank Points");
      waypoints.restoreBlankPoints();
    }
  }
  frontControls.setSwitchCallbacks(upSwitchPressed, downSwitchPressed, enterSwitchPressed);
  frontControls.setSwitchHeldCallbacks(upSwitchHeld, downSwitchHeld);
  gpsGuide.setSerialTokenReleasedCallback(gpsSerialReleased);
  if (adminMode == YES) {
    beeper.useBuzzer = YES;
    beeper.volume = 5;
    beeper.beepWithRate(10);
    beeper.beepAcknowledge();
    frontControls.generalMessage("Admin mode");
  } else {
    beeper.beepAffirmative();
    frontControls.generalMessage("Begin Journey");
    loadWaypoint(0);
  }
  //selfTest();
}

void loop() {
  beeper.processLoop();
  gpsGuide.processLoop();
  frontControls.processLoop();
}

void enterSwitchPressed() {
  if(!adminMode) return;
  float lat, lon;
  boolean goodPosition = gpsGuide.getPosition(&lat, &lon);
  if (goodPosition) {
    waypoints.storeWaypoint(currentWaypoint(), lat, lon);
    loadWaypoint(currentWaypoint());
  }
}

void upSwitchPressed() {
  if(!adminMode) return;
  if (currentMenuIndex < 19) {
    ++currentMenuIndex;
    frontControls.displayNumber(currentMenuIndex);
    loadWaypoint(currentMenuIndex - 1);
  }
}

void downSwitchPressed() {
  if(!adminMode) return;
  if (currentMenuIndex > 0) {
    --currentMenuIndex;
    frontControls.displayNumber(currentMenuIndex);
    loadWaypoint(currentMenuIndex - 1);
  }
}

int currentWaypoint() {
  return currentMenuIndex - 1;
}

void upSwitchHeld() {
  
  
}

void downSwitchHeld() {
   
}

void gpsSerialReleased() {
  //debugPrintln("Giving token to scanner");
  gpsGuide.serialTokenReturned();
}


void loadWaypoint(int point) {
  if (point < 0) {
    gpsGuide.endGuiding();
    return;
  }
  gpsGuide.endGuiding();
  float lat, lon;
  waypoints.retrieveWaypoint(point, &lat, &lon);
  frontControls.displayWaypointSelection(point + 1, lat, lon);
  if (lat == TinyGPS::GPS_INVALID_F_ANGLE || lon == TinyGPS::GPS_INVALID_F_ANGLE) {
    debugPrintln("INVALID WAYPOINT");
    //frontControls.setStatusLight(ON);
    return; 
  } else {
    //frontControls.setStatusLight(OFF);
  }
  gpsGuide.setDestination(lat, lon);
  gpsGuide.beginGuiding();
}

void selfTest() {
  debugPrintln("Running self test");
  //beeper.selfTest();
  frontControls.selfTest();
  gpsGuide.setDestination(100, 100);
  gpsGuide.beginGuiding();
}