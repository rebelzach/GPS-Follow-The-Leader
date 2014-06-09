

#ifndef WAYPOINTSTORE_h
#define WAYPOINTSTORE_h

#include "Arduino.h"

class WaypointStore
{
  public:
    
    // methods
    WaypointStore();
    void retrieveWaypoint(int waypoint, float *lat, float *lon);
    void storeWaypoint(int waypoint, float lat, float lon);
    void restorePresetPoints();
    void restoreBlankPoints();
    void dumpWaypoints();
  private:
    
     
};

#endif
