

#ifndef GPSGUIDE_h
#define GPSGUIDE_h

#include "Arduino.h"

class GPSGuide
{
  public:
    boolean displayInfo;
    
    // methods
    GPSGuide();
    boolean getPosition(float *lat, float *lon);
    void processLoop();
    void setDestination(float lat, float lon);
    float distanceFromDestination();
    void beginGuiding();
    void endGuiding();
    void setFixChangedCallback(void (*fixChangedCallback)(boolean));
    void setSerialTokenReleasedCallback(void (*serialTokenReleasedCallback)());
    void serialTokenReturned();
    
  private:
    boolean haveSerialToken;
    void returnSerialToken();
    void (*fixChangedCallback)(boolean);
    void (*serialTokenReleasedCallback)();
    float destinationLat;
    float destinationLon; 
    float refLat;
    float refLon; 
    boolean guiding;
    int hotColdLevel;
    
    boolean isPositionValid();
    float courseToDestination();
    boolean movedFarEnoughToUpdateStatus();
    void storeNewMovementReference();
    void displayGPSInfo();
    void invalidateMovementReference();
    float currentCourseFromReference();
    boolean courseWithinDegressOfCourse(float course1, float course2, float tolerance);
    void updateLastTrackedDistance();
    void updateGuide();
    void updateLevel(int level);
};

#endif
