#include "Sevi160RCController.h"

namespace Sevi160RCControllerNs {
Sevi16RCFanSpeed currentFanSpeed = Sevi16RCFanSpeed::UnknownFanSpeed;
Sevi16RCVentilationMode currentVentilationMode =
    Sevi16RCVentilationMode::UnknownVentilationMode;
int filterResetLedPin = -1;
bool filterChangeRequired = false;
void setRCControllerFanSpeed(Sevi16RCFanSpeed fanSpeed) {
    currentFanSpeed = fanSpeed;
}
void setCurrentVentilationMode(Sevi16RCVentilationMode ventilationMode) {
    currentVentilationMode = ventilationMode;
}
void setFilterChangeRequired(bool changeRequired) {
    filterChangeRequired = changeRequired;
}
void IRAM_ATTR setFanSpeed1() {
    setRCControllerFanSpeed(Sevi16RCFanSpeed::One);
}
void IRAM_ATTR setFanSpeed2() {
    setRCControllerFanSpeed(Sevi16RCFanSpeed::Two);
}
void IRAM_ATTR setFanSpeed3() {
    setRCControllerFanSpeed(Sevi16RCFanSpeed::Three);
}
void IRAM_ATTR setFanSpeed4() {
    setRCControllerFanSpeed(Sevi16RCFanSpeed::Four);
}
void IRAM_ATTR setHrvVentilationMode() {
    setCurrentVentilationMode(Sevi16RCVentilationMode::HRV);
}
void IRAM_ATTR setBypassVentilationMode() {
    setCurrentVentilationMode(Sevi16RCVentilationMode::Bypass);
}
void IRAM_ATTR setFilterChangeRequired() {
    if (filterResetLedPin != -1) {
        setFilterChangeRequired(digitalRead(filterResetLedPin));
    }
}
}  // namespace Sevi160RCControllerNs

Sevi160RCController::Sevi160RCController(Sevi160RCIOConfig config) {
    Sevi160RCController::ioConfig = config;
    pinMode(ioConfig.fanSpeed1LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed1LedPin),
                    Sevi160RCControllerNs::setFanSpeed1, HIGH);
    pinMode(ioConfig.fanSpeed2LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed2LedPin),
                    Sevi160RCControllerNs::setFanSpeed2, HIGH);
    pinMode(ioConfig.fanSpeed3LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed3LedPin),
                    Sevi160RCControllerNs::setFanSpeed3, HIGH);
    pinMode(ioConfig.fanSpeed4LedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.fanSpeed4LedPin),
                    Sevi160RCControllerNs::setFanSpeed4, HIGH);

    pinMode(ioConfig.hrvModeLedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.hrvModeLedPin),
                    Sevi160RCControllerNs::setHrvVentilationMode, HIGH);
    pinMode(ioConfig.bypassModeLedPin, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(ioConfig.bypassModeLedPin),
                    Sevi160RCControllerNs::setBypassVentilationMode, HIGH);

    pinMode(ioConfig.filterResetLedPin, INPUT_PULLDOWN);
    Sevi160RCControllerNs::filterResetLedPin = ioConfig.filterResetLedPin;
    attachInterrupt(digitalPinToInterrupt(ioConfig.filterResetLedPin),
                    Sevi160RCControllerNs::setFilterChangeRequired, CHANGE);

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
}

Sevi16RCFanSpeed Sevi160RCController::getCurrentFanSpeed() {
    return Sevi160RCControllerNs::currentFanSpeed;
}

Sevi16RCVentilationMode Sevi160RCController::getCurrentVentilationMode() {
    return Sevi160RCControllerNs::currentVentilationMode;
}

bool Sevi160RCController::getFilterChangeRequired() {
    return Sevi160RCControllerNs::filterChangeRequired;
}

bool Sevi160RCController::isControlPanelAwake() {
    return digitalRead(ioConfig.hrvModeLedPin) == HIGH ||
           digitalRead(ioConfig.bypassModeLedPin) == HIGH;
}

void Sevi160RCController::ensureControlPanelIsAwake() {
    while (!isControlPanelAwake()) {
        awakeControlPanel();
    }
}

void Sevi160RCController::awakeControlPanel() {
    sendControlPanelCommand(ioConfig.filterResetButtonPin, false);
}

void Sevi160RCController::sendControlPanelCommand(
    int buttonPin, bool ensureControlPanelIsAwake, int commandTime) {
    /**
     * give the panel time to boot
     * if commands are sent before the panel has the chance to boot, it will get stuck at boot
     */
    if (millis() < 10000) {
        return;
    }
    if (ensureControlPanelIsAwake) {
        Sevi160RCController::ensureControlPanelIsAwake();
    }
    digitalWrite(buttonPin, 0);
    delay(commandTime);
    digitalWrite(buttonPin, 1);
    delay(commandTime);
}

void Sevi160RCController::changeFanSpeed(Sevi16RCFanSpeed fanSpeed) {
    Sevi16RCFanSpeed currentFanSpeed = getCurrentFanSpeed();
    while (currentFanSpeed != fanSpeed) {
        sendControlPanelCommand(ioConfig.fanSpeedButtonPin);
        currentFanSpeed = getCurrentFanSpeed();
    }
}

void Sevi160RCController::changeVentilationMode(Sevi16RCVentilationMode mode) {
    if (mode == Sevi16RCVentilationMode::HRV) {
        sendControlPanelCommand(ioConfig.hrvModeButtonPin);
    } else {
        sendControlPanelCommand(ioConfig.bypassModeButtonPin);
    }
}

void Sevi160RCController::resetFilterChangeStatus() {
    sendControlPanelCommand(ioConfig.filterResetButtonPin);
}

void Sevi160RCController::toggleOnOff() {
    sendControlPanelCommand(ioConfig.powerButtonPin, true, 6000);
}

Sevi160RCStatus Sevi160RCController::getStatus() {
    Sevi160RCStatus status = {
        .fanSpeed = getCurrentFanSpeed(),
        .mode = getCurrentVentilationMode(),
        .filterChangeRequired = getFilterChangeRequired()};
    return status;
}