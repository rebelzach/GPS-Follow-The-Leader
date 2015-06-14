

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
    void restoreMeadowPoints();
    void restoreGeocachePoints();
    void restoreBlankPoints();
    void dumpWaypoints();
  private:
    
     
};

#endif
