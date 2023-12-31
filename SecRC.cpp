#include "SecRC.h"

namespace SecRCNs {
SecRCFanSpeed currentFanSpeed = SecRCFanSpeed::UnknownFanSpeed;
SecRCVentilationMode currentVentilationMode =
    SecRCVentilationMode::UnknownVentilationMode;
int filterResetLedPin = -1;
bool filterChangeRequired = false;
void setRCControllerFanSpeed(SecRCFanSpeed fanSpeed) {
    currentFanSpeed = fanSpeed;
}
void setCurrentVentilationMode(SecRCVentilationMode ventilationMode) {
    currentVentilationMode = ventilationMode;
}
void setFilterChangeRequired(bool changeRequired) {
    filterChangeRequired = changeRequired;
}

void wait(int msec, int startTime = 0) {
    if (startTime == 0) {
        startTime = millis();
    }
    unsigned int elapsed = millis() - startTime;
    if (elapsed < msec) {
        return wait(msec, startTime);
    }
    return;
}

void checkFilterChangeStatus() {
    if (filterResetLedPin != -1) {
        wait(10);
        setFilterChangeRequired(digitalRead(filterResetLedPin));
    }
}

void IRAM_ATTR setFanSpeed1() { setRCControllerFanSpeed(SecRCFanSpeed::One); }
void IRAM_ATTR setFanSpeed2() { setRCControllerFanSpeed(SecRCFanSpeed::Two); }
void IRAM_ATTR setFanSpeed3() { setRCControllerFanSpeed(SecRCFanSpeed::Three); }
void IRAM_ATTR setFanSpeed4() { setRCControllerFanSpeed(SecRCFanSpeed::Four); }
void IRAM_ATTR setHrvVentilationMode() {
    setCurrentVentilationMode(SecRCVentilationMode::HRV);
    checkFilterChangeStatus();
}
void IRAM_ATTR setBypassVentilationMode() {
    setCurrentVentilationMode(SecRCVentilationMode::Bypass);
    checkFilterChangeStatus();
}
}  // namespace SecRCNs

SecRC::SecRC(SecRCIOConfig config) {
    SecRC::ioConfig = config;
    pinMode(ioConfig.fanSpeed1LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed1LedPin),
                    SecRCNs::setFanSpeed1, HIGH);
    pinMode(ioConfig.fanSpeed2LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed2LedPin),
                    SecRCNs::setFanSpeed2, HIGH);
    pinMode(ioConfig.fanSpeed3LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed3LedPin),
                    SecRCNs::setFanSpeed3, HIGH);
    pinMode(ioConfig.fanSpeed4LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed4LedPin),
                    SecRCNs::setFanSpeed4, HIGH);

    pinMode(ioConfig.hrvModeLedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.hrvModeLedPin),
                    SecRCNs::setHrvVentilationMode, HIGH);
    pinMode(ioConfig.bypassModeLedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.bypassModeLedPin),
                    SecRCNs::setBypassVentilationMode, HIGH);

    pinMode(ioConfig.filterResetLedPin, INPUT_PULLDOWN);
    SecRCNs::filterResetLedPin = ioConfig.filterResetLedPin;

    pinMode(ioConfig.enablePin, OUTPUT);
    digitalWrite(ioConfig.enablePin, LOW);
    pinMode(ioConfig.powerButtonPin, OUTPUT);
    digitalWrite(ioConfig.powerButtonPin, 1);
    pinMode(ioConfig.hrvModeButtonPin, OUTPUT);
    digitalWrite(ioConfig.hrvModeButtonPin, 1);
    pinMode(ioConfig.bypassModeButtonPin, OUTPUT);
    digitalWrite(ioConfig.bypassModeButtonPin, 1);
    pinMode(ioConfig.fanSpeedButtonPin, OUTPUT);
    digitalWrite(ioConfig.fanSpeedButtonPin, 1);
    pinMode(ioConfig.filterResetButtonPin, OUTPUT);
    digitalWrite(ioConfig.filterResetButtonPin, 1);
    this->bootUp();
}

SecRCFanSpeed SecRC::getCurrentFanSpeed() { return SecRCNs::currentFanSpeed; }

SecRCVentilationMode SecRC::getCurrentVentilationMode() {
    return SecRCNs::currentVentilationMode;
}

void SecRC::bootUp() {
    // wait for Arduino boot (avoid boot-up glitches)
    delay(3000);
    // power-up the panel
    digitalWrite(ioConfig.enablePin, HIGH);
    delay(3000);
    // start up the panel
    sendControlPanelCommand(ioConfig.powerButtonPin, false, 3000);
}

bool SecRC::getFilterChangeRequired() { return SecRCNs::filterChangeRequired; }

bool SecRC::isControlPanelAwake() {
    return digitalRead(ioConfig.hrvModeLedPin) == HIGH ||
           digitalRead(ioConfig.bypassModeLedPin) == HIGH;
}

void SecRC::ensureControlPanelIsAwake() {
    while (!isControlPanelAwake()) {
        awakeControlPanel();
    }
}

void SecRC::awakeControlPanel() {
    sendControlPanelCommand(ioConfig.filterResetButtonPin, false);
}

void SecRC::sendControlPanelCommand(int buttonPin,
                                    bool ensureControlPanelIsAwake,
                                    int commandTime) {
    if (ensureControlPanelIsAwake) {
        SecRC::ensureControlPanelIsAwake();
    }
    digitalWrite(buttonPin, 0);
    delay(commandTime);
    digitalWrite(buttonPin, 1);
    delay(commandTime);
}

void SecRC::changeFanSpeed(SecRCFanSpeed fanSpeed) {
    ensureControlPanelIsAwake();
    SecRCFanSpeed currentFanSpeed = getCurrentFanSpeed();
    while (currentFanSpeed != fanSpeed) {
        sendControlPanelCommand(ioConfig.fanSpeedButtonPin);
        currentFanSpeed = getCurrentFanSpeed();
    }
}

void SecRC::changeVentilationMode(SecRCVentilationMode mode) {
    if (mode == SecRCVentilationMode::HRV) {
        sendControlPanelCommand(ioConfig.hrvModeButtonPin);
    } else {
        sendControlPanelCommand(ioConfig.bypassModeButtonPin);
    }
}

void SecRC::resetFilterChangeStatus() {
    sendControlPanelCommand(ioConfig.filterResetButtonPin, true, 6000);
}

void SecRC::toggleOnOff() {
    sendControlPanelCommand(ioConfig.powerButtonPin, true, 6000);
}

SecRCStatus SecRC::getStatus(bool sync) {
    if (sync) {
        ensureControlPanelIsAwake();
    }
    SecRCStatus status = {.fanSpeed = getCurrentFanSpeed(),
                          .mode = getCurrentVentilationMode(),
                          .filterChangeRequired = getFilterChangeRequired()};
    return status;
}