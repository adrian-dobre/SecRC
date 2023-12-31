#ifndef SecRC_h
#define SecRC_h

#include "Arduino.h"

struct SecRCIOConfig {
    int enablePin;
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

enum SecRCFanSpeed {
    UnknownFanSpeed = 0,
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4
};

enum SecRCVentilationMode { UnknownVentilationMode = 0, HRV = 1, Bypass = 2 };

struct SecRCStatus {
    SecRCFanSpeed fanSpeed;
    SecRCVentilationMode mode;
    bool filterChangeRequired;
};

class SecRC {
   public:
    SecRC(SecRCIOConfig ioConfig);
    void changeFanSpeed(SecRCFanSpeed fanSpeed);
    void changeVentilationMode(SecRCVentilationMode ventilationMode);
    void resetFilterChangeStatus();
    void toggleOnOff();
    SecRCStatus getStatus(bool sync = false);

   private:
    SecRCIOConfig ioConfig;
    bool isControlPanelAwake();
    void awakeControlPanel();
    void ensureControlPanelIsAwake();
    void sendControlPanelCommand(int buttonPin,
                                 bool ensureControlPanelIsAwake = true,
                                 int commandTime = 300);
    SecRCFanSpeed getCurrentFanSpeed();
    SecRCVentilationMode getCurrentVentilationMode();
    bool getFilterChangeRequired();
    void bootUp();
};

#endif