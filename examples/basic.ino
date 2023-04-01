#include <Sevi160RCController.h>

Sevi160RCController *controller;

void setup() {
    Serial.begin(115200);
    Sevi160RCIOConfig config = {.fanSpeed1LedPin = 34,
                                .fanSpeed2LedPin = 35,
                                .fanSpeed3LedPin = 36,
                                .fanSpeed4LedPin = 39,
                                .hrvModeLedPin = 33,
                                .bypassModeLedPin = 32,
                                .filterResetLedPin = 27,
                                .powerButtonPin = 13,
                                .fanSpeedButtonPin = 26,
                                .hrvModeButtonPin = 25,
                                .bypassModeButtonPin = 14,
                                .filterResetButtonPin = 4};
    controller = new Sevi160RCController(config);
}

void loop() {
    controller->changeVentilationMode(Sevi16RCVentilationMode::HRV);
    controller->changeFanSpeed(Sevi16RCFanSpeed::Three);
    // controller->resetFilterChangeStatus();
    // controller->toggleOnOff();
    Sevi160RCStatus controllerStatus = controller->getStatus();
    Serial.print("Mode: ");
    Serial.print(controllerStatus.mode);
    Serial.print(" Fan Speed: ");
    Serial.print(controllerStatus.fanSpeed);
    Serial.print(" Filter change required: ");
    Serial.println(controllerStatus.filterChangeRequired);
    delay(60000);
}