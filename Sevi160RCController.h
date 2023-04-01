#ifndef Sevi160RCController_h
#define Sevi160RCController_h

#include "Arduino.h"

struct Sevi160RCIOConfig {
    int fanSpeed1LedPin;
    int fanSpeed2LedPin;
    int fanSpeed3LedPin;
    int fanSpeed4LedPin;
    int hrvModeLedPin;
    int bypassModeLedPin;
    int filterResetLedPin;
    int powerButtonPin;
    int fanSpeedButtonPin;
    int hrvModeButtonPin;
    int bypassModeButtonPin;
    int filterResetButtonPin;
};

enum Sevi16RCFanSpeed {
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4
};

enum Sevi16RCVentilationMode { HRV = 1, Bypass = 2 };

struct Sevi160RCStatus {
    Sevi16RCFanSpeed fanSpeed;
    Sevi16RCVentilationMode mode;
    bool filterChangeRequired;
};

class Sevi160RCController {
   public:
    Sevi160RCController(Sevi160RCIOConfig ioConfig);
    void changeFanSpeed(Sevi16RCFanSpeed fanSpeed);
    void changeVentilationMode(Sevi16RCVentilationMode ventilationMode);
    void resetFilterChangeStatus();
    void toggleOnOff();
    Sevi160RCStatus getStatus();

   private:
    Sevi160RCIOConfig ioConfig;
    bool isControlPanelAwake();
    void awakeControlPanel();
    void ensureControlPanelIsAwake();
    void sendControlPanelCommand(int buttonPin,
                                 bool ensureControlPanelIsAwake = true,
                                 int commandTime = 300);
    Sevi16RCFanSpeed getCurrentFanSpeed();
    Sevi16RCVentilationMode getCurrentVentilationMode();
    bool getFilterChangeRequired();
};

#endif