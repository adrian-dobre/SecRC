#include <Sevi160RCController.h>

Sevi160RCController *controller;

void setup() {
    Serial.begin(115200);
    Sevi160RCIOConfig config = {.fanSpeed1LedPin = 1,
                                .fanSpeed2LedPin = 2,
                                .fanSpeed3LedPin = 3,
                                .fanSpeed4LedPin = 4,
                                .hrvModeLedPin = 5,
                                .bypassModeLedPin = 6,
                                .filterResetLedPin = 7,
                                .fanSpeedButtonPin = 8,
                                .hrvModeButtonPin = 9,
                                .bypassModeButtonPin = 10,
                                .filterResetButtonPin = 11,
                                .powerButtonPin = 12};
    controller = new Sevi160RCController(config);
}

void loop() {
    controller->changeVentilationMode(Sevi16RCVentilationMode.HRV);
    controller->changeFanSpeed(Sevi16RCFanSpeed.One);
    // controller->resetFilterChangeStatus();
    // controller->toggleOnOff();
    Sevi160RCStatus controllerStatus = controller->getStatus();
    Serial.print("Mode: ");
    Serial.print(controllerStatus.mode);
    Serial.print(" Fan Speed: ");
    Serial.print(controllerStatus.fanSpeed);
    Serial.print(" Filter change required: ");
    Serial.print(controllerStatus.filterChangeRequired ? "Yes" : "No");
    delay(3000);
}