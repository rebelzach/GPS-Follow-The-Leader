
#ifndef GPSGUIDE_cpp
#define GPSGUIDE_cpp

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "Definitions.h"
#include "GPSGuide.h"
#include "Beeper.h"

LazyTimer(SerialTimeoutTimer);
SoftwareSerial gpsSerial(2, 3);
TinyGPS gps;
int debugMovementUpdates = 0;
int debugDistanceFromRef = 0;
int debugBeepRate = 0;
float debugCourse = 0;
float debugDestCourse = 0;

const unsigned long HDOP_UNACCEPTABLE_THRESHOLD = 250;
const float GPS_INVALID_DISTANCE = 1000000.0;
const int GPS_MAX_INVALID_TIME = 5000;
const int FAR_ANGLE_TOLERANCE = 60;
const int CLOSE_ANGLE_DISTANCE = 30;
const int CLOSE_ANGLE_TOLERANCE = 50;
#if 0 // backyard constants
const int GPS_TRACKING_DISTANCE = 3;
const int ARRIVAL_THRESHOLD = 2;
const int BEEP_DISTANCE_SCALAR = 20; // This is the max distance that the beep rate will be calculated against
#else
const int GPS_TRACKING_DISTANCE = 5;
const int ARRIVAL_THRESHOLD = 20;
const int BEEP_DISTANCE_SCALAR = 1000; // This is the max distance that the beep rate will be calculated against
#endif

const int LEVEL_NOSIGNAL= -2;
const int LEVEL_OFFCOURSE= -1;
const int LEVEL_ONCOURSE= 0;
const int LEVEL_ARRIVED= 1;

GPSGuide::GPSGuide() {
  destinationLat = TinyGPS::GPS_INVALID_F_ANGLE;
  destinationLon = TinyGPS::GPS_INVALID_F_ANGLE; 
  refLat = TinyGPS::GPS_INVALID_F_ANGLE;
  refLon = TinyGPS::GPS_INVALID_F_ANGLE; 
  guiding = NO;
  hotColdLevel = -100;
  gpsSerial.begin(4800);
  haveSerialToken = NO;
  displayInfo = NO;
}

void GPSGuide::setDestination(float lat, float lon) {
  if (guiding) {
    debugPrintln("Didn't expect to have destination set while guiding");
  }
  destinationLat = lat;
  destinationLon = lon;
}

void GPSGuide::beginGuiding() {
  if (guiding)
    return;
  guiding = YES;
  gpsSerial.listen();
  updateLevel(LEVEL_OFFCOURSE);
  // Enable WAAS
  gpsSerial.print("$PSRF151,01*0F"); gpsSerial.write(13); gpsSerial.write(10);
  //VTG ON, Tiny GPS doesn't use this but it would be good to experiment with its values
  gpsSerial.print("$PSRF103,05,00,01,01*20"); gpsSerial.write(13); gpsSerial.write(10); 
  invalidateMovementReference();
#if 0
  debugPrintln("tests");
  debugPrintln(courseWithinDegressOfCourse(350, 2, 25)); // true
  debugPrintln(courseWithinDegressOfCourse(2, 350, 25)); // true
  debugPrintln(courseWithinDegressOfCourse(5, 350, 5));  // false
  debugPrintln(courseWithinDegressOfCourse(5, 175, 5));  // false
  debugPrintln(courseWithinDegressOfCourse(180, 175, 30)); //true
  debugPrintln(courseWithinDegressOfCourse(180, 185, 30)); //true
#endif
}

void GPSGuide::serialTokenReturned() {
  //gpsSerial.listen();
  haveSerialToken = YES;
}

void GPSGuide::setSerialTokenReleasedCallback(void (*callback)()) {
  serialTokenReleasedCallback = callback;
}

void GPSGuide::endGuiding() {
  if (!guiding)
    return;
  guiding = NO;
  beeper.stopBeeping();
}

float GPSGuide::distanceFromDestination() {
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  if (age < GPS_MAX_INVALID_TIME) {
    return gps.distance_between(flat, flon, destinationLat, destinationLon);
  } else {
    return GPS_INVALID_DISTANCE;
  }
}

float GPSGuide::courseToDestination() {
  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  return gps.course_to(flat, flon, destinationLat, destinationLon);
}

void GPSGuide::processLoop() {
//  if (!haveSerialToken) {
//    return;
//  }
  
  boolean newData = false;
  while (gpsSerial.available()) {
    ResetLazyTimer(SerialTimeoutTimer);
    char c = gpsSerial.read();
    //Serial.write(c);
    if (gps.encode(c))
      newData = true;
  }
  
  if (newData)
  {
    if (guiding) {
      if (adminMode) {
        displayGPSInfo();
      }
      updateGuide();
    }
#if 0
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    debugPrint("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    debugPrint(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    debugPrint(" SAT=");
    debugPrint(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    debugPrint(" PREC=");
    debugPrintln(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    unsigned long chars;
    unsigned short sentences, failed;
    gps.stats(&chars, &sentences, &failed);
    debugPrint(" SENTENCES=");
    debugPrintln(sentences);
    if (guiding) {
      debugPrint("Distance from dest:");
      debugPrintln(gps.distance_between(flat, flon, destinationLat, destinationLon));
    }
#endif
  }
  if (LazyTimerPastDuration(SerialTimeoutTimer, 20)) {
    returnSerialToken();
  }
}

void GPSGuide::returnSerialToken() {
  StopLazyTimer(SerialTimeoutTimer);
  haveSerialToken = NO;
  serialTokenReleasedCallback();
}

boolean GPSGuide::courseWithinDegressOfCourse(float course1, float course2, float tolerance) {
  float angleDifference;
  if (fabs(course1 - course2) > 180) {
    angleDifference = course1 - 360 + course2;
  } else {
    angleDifference = course1 - course2;
  }
  return (fabs(angleDifference) < tolerance);
}

void GPSGuide::updateGuide() {
  if (!isPositionValid()) {
    frontControls.displayNoFix();
    return;
  }
  
  float currentDistance = distanceFromDestination();
  
  boolean arrived = NO;
  if (currentDistance < ARRIVAL_THRESHOLD) {
    updateLevel(LEVEL_ARRIVED);
    arrived = YES;
  }
  
  if (!movedFarEnoughToUpdateStatus()) {
    return;
  }
  
  beeper.beepAcknowledge();
  ++debugMovementUpdates;
  
  float currentCourse = currentCourseFromReference(); // get this before resetting reference
  debugCourse = currentCourse;
  float courseToDest = courseToDestination();
  debugDestCourse = courseToDest;
  //frontControls.updateGuideInfo(currentCourse, courseToDest, currentDistance);
  storeNewMovementReference();

  
  if (arrived)
    return;
  

  int degreeTolerance = FAR_ANGLE_TOLERANCE;
  if (currentDistance < CLOSE_ANGLE_DISTANCE) {
    degreeTolerance = CLOSE_ANGLE_TOLERANCE;
  }
  
  if (!courseWithinDegressOfCourse(currentCourse, courseToDest, degreeTolerance)) {
    debugPrintln("off course");
    updateLevel(LEVEL_OFFCOURSE);
    return;
  }
  updateLevel(LEVEL_ONCOURSE);
}

void GPSGuide::updateLevel(int level) {
  if (hotColdLevel == level)
    return;
  hotColdLevel = level;
  
  switch (level) {
    case LEVEL_OFFCOURSE:
      debugBeepRate = 0;
      beeper.stopBeeping();
      frontControls.guideOffcourse();
      break;
    case LEVEL_ONCOURSE:
    {
      int beepRate = distanceFromDestination()/BEEP_DISTANCE_SCALAR*10;
      debugBeepRate = beepRate;
      beeper.beepWithRate(beepRate);
    }
      break;
    case LEVEL_ARRIVED:
      debugBeepRate = 99;
      beeper.beepReallyFast();
      frontControls.guideMessage("You have arrived.");
      break;
  }
}

boolean GPSGuide::getPosition(float *lat, float *lon) {
  unsigned long age;
  gps.f_get_position(lat, lon, &age);
  if (age < GPS_MAX_INVALID_TIME && *lat != TinyGPS::GPS_INVALID_F_ANGLE && *lon != TinyGPS::GPS_INVALID_F_ANGLE) {
    unsigned long hdopVal = gps.hdop();
    if (hdopVal == TinyGPS::GPS_INVALID_HDOP || hdopVal > HDOP_UNACCEPTABLE_THRESHOLD) {
      return NO;
    }
    return YES;
  }
  return NO;
}

boolean GPSGuide::isPositionValid() {
  float lat;
  float lon;
  return getPosition(&lat, &lon);
}

boolean GPSGuide::movedFarEnoughToUpdateStatus() {
  if (refLat == TinyGPS::GPS_INVALID_F_ANGLE || refLon == TinyGPS::GPS_INVALID_F_ANGLE) {
    storeNewMovementReference();
    return NO;
  }
  float flat, flon;
  boolean goodPos = getPosition(&flat, &flon);
  if(goodPos) {
    float d = gps.distance_between(refLat, refLon, flat, flon);
    debugDistanceFromRef = d;
    if (d > GPS_TRACKING_DISTANCE)
      return YES;
  }
  return NO;
}

void GPSGuide::storeNewMovementReference() {
  float flat, flon;
  boolean good = getPosition(&flat, &flon);
  if (good) {
    debugPrintln("Storing ref");
    refLat = flat;
    refLon = flon;
  }
}

float GPSGuide::currentCourseFromReference() {
  float flat, flon;
  boolean good = getPosition(&flat, &flon);
  if (good) {
    return gps.course_to(refLat, refLon, flat, flon);
  } else {
    return TinyGPS::GPS_INVALID_F_ANGLE;
  }
}

void GPSGuide::invalidateMovementReference() {
  refLat = TinyGPS::GPS_INVALID_F_ANGLE;
  refLon = TinyGPS::GPS_INVALID_F_ANGLE; 
}

void GPSGuide::displayGPSInfo() {
  float lat;
  float lon;
  getPosition(&lat, &lon);
  frontControls.displayGPS(lat,lon,distanceFromDestination(), gps.hdop(), debugMovementUpdates , debugDistanceFromRef, debugCourse, debugDestCourse, debugBeepRate);
}

#endif

