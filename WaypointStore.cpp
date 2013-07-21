
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
  float waypoint[10][2] = 
  {{39.64516,-105.62154},
  {39.64500,-105.62163},
  {39.64472,-105.62129},
  {39.64482,-105.62054},
  {39.64510,-105.62061},
  {39.64521,-105.62053},
  {39.64569,-105.62046},
  {39.64619,-105.62039},
  {39.64648,-105.62031},
  {39.646228,-105.621467}};
  
  for (int i = 0; i < 10; i++) {
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

