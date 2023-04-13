#include <SecRC.h>

SecRC *secRc;

void setup() {
    Serial.begin(115200);
    SecRCIOConfig config = {.fanSpeed1LedPin = 34,
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
    secRc = new SecRC(config);
}

void loop() {
    secRc->changeVentilationMode(SecRCVentilationMode::HRV);
    secRc->changeFanSpeed(SecRCFanSpeed::Three);
    // secRc->resetFilterChangeStatus();
    // secRc->toggleOnOff();
    SecRCStatus panelStatus = secRc->getStatus();
    Serial.print("Mode: ");
    Serial.print(panelStatus.mode);
    Serial.print(" Fan Speed: ");
    Serial.print(panelStatus.fanSpeed);
    Serial.print(" Filter change required: ");
    Serial.println(panelStatus.filterChangeRequired);
    delay(60000);
}
