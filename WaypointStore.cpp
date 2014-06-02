
#ifndef WAYPOINTSTORE_cpp
#define WAYPOINTSTORE_cpp

#include <EEPROM.h>
#include "Definitions.h"
#include "WaypointStore.h"
#include <TinyGPS.h>

int addressForWaypoint(int waypoint);
float eepromReadFloat(int address);
void eepromWriteFloat(int address, float value);

WaypointStore::WaypointStore() {

}

void WaypointStore::restorePresetPoints()
{
  float waypoint[16][2] = 
  {
  {39.67392079097836,-105.6117598536543},
  {39.67312967695812,-105.6137704895044},
  {39.6725974325415,-105.6149705031519},
  {39.67068149070244,-105.6157400897967},
  {39.66992161431844,-105.616714541875},
  {39.6691754829238,-105.6178531317114},
  {39.66876590718994,-105.619203849446},
  {39.66703987926044,-105.61942396896},
  {39.66448954784703,-105.6184292798787},
  {39.66248719505322,-105.6187391363614},
  {39.66109171986825,-105.6198887550013},
  {39.6595153621529,-105.6202879274368},
  {39.6568131332338,-105.6205685825051},
  {39.65335172178366,-105.6202475008714},
  {39.64812346523554,-105.6201016913099},
  {39.6462335131115,-105.6209625339392}
};
  
  for (int i = 0; i < 16; i++) {
    storeWaypoint(i, waypoint[i][0], waypoint[i][1]);
  }
}

void WaypointStore::restoreBlankPoints()
{
  for (int i = 0; i < 19; i++) {
    storeWaypoint(i, TinyGPS::GPS_INVALID_F_ANGLE, TinyGPS::GPS_INVALID_F_ANGLE);
  }
}

void WaypointStore::retrieveWaypoint(int waypoint, float *lat, float *lon)
{
  int latAddress = addressForWaypoint(waypoint);
  int lonAddress = latAddress + 4;
  *lat = eepromReadFloat(latAddress);
  *lon = eepromReadFloat(lonAddress);
}

void WaypointStore::storeWaypoint(int waypoint, float lat, float lon)
{
  int latAddress = addressForWaypoint(waypoint);
  if (eepromReadFloat(latAddress) != lat) {
    eepromWriteFloat(latAddress, lat);
  }
  int lonAddress = latAddress + 4;
  if (eepromReadFloat(lonAddress) != lon) {
    eepromWriteFloat(lonAddress, lon);
  }
  debugPrintln("Storing");
}

int addressForWaypoint(int waypoint)
{
  int address = waypoint * 4 * 2;
  return address;
}
 
float eepromReadFloat(int address)
{
   union u_tag {
     byte b[4];
     float fval;
   } u;   
   u.b[0] = EEPROM.read(address);
   u.b[1] = EEPROM.read(address+1);
   u.b[2] = EEPROM.read(address+2);
   u.b[3] = EEPROM.read(address+3);
   return u.fval;
}
 
void eepromWriteFloat(int address, float value)
{
   union u_tag {
     byte b[4];
     float fval;
   } u;
   u.fval=value;
 
   EEPROM.write(address  , u.b[0]);
   EEPROM.write(address+1, u.b[1]);
   EEPROM.write(address+2, u.b[2]);
   EEPROM.write(address+3, u.b[3]);
}

#endif

