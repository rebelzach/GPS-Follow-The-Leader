
#ifndef FRONTCONTROLS_h
#define FRONTCONTROLS_h

#include "Arduino.h"
#include <TinyGPS.h>

enum Switches {
  UP_SWITCH,
  DOWN_SWITCH,
  ENTER_SWITCH
};

enum SwitchStatus {
  HELD,
  PUSHED
};

class FrontControls
{
  public:
    
    // methods
    FrontControls();
    void begin();
    boolean getPosition(float *lat, float *lon);
    void setStatusLight(boolean lightState);
    void pulseStatusLight(boolean shouldPulse);
    void displayNumber(int number);
    void processLoop();
    void setSwitchCallbacks(void (*upCallback)(),void (*downCallback)(), void (*enterCallback)());
    void setSwitchHeldCallbacks(void (*upHeldCallback)(),void (*downHeldCallback)());
    void handleSwitch(Switches theSwitch, SwitchStatus theStatus);
    boolean upSwitchState();
    boolean downSwitchState();
    void selfTest();
    void generalMessage(String message);
    void guideMessage(String message);
    void guideHasFix();
    void guideOffcourse();
    void displayWaypointSelection(int waypointIndex, float lat, float lon);
    void displayNoFix();
    void displayGPS(float lat, float lon, float distance, unsigned long precision, int updates, float dFromRef, float recentCourse, float destCourse, int beepRate);
    void updateGuideInfo(float currentCourse, float courseToDest, float currentDistance);
    void activateBacklight();
    void clear();
  private:
    boolean haveFix;
    void (*upSwitchCallback)();
    void (*downSwitchCallback)();
    void (*enterSwitchCallback)();
    void (*upSwitchHeldCallback)();
    void (*downSwitchHeldCallback)();
    
    void checkSwitchPin(int pin);
    boolean isSelfTesting;
};

#endif
