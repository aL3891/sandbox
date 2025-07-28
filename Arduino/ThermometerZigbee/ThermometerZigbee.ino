#include "Zigbee.h"

uint8_t temp1Pin = A0;
uint8_t button = BOOT_PIN;
ZigbeeAnalog zbTemp1 = ZigbeeAnalog(1);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(button, INPUT_PULLUP);

  zbTemp1.setManufacturerAndModel("Allan", "ZigbeeTemp1");
  zbTemp1.setAnalogInputApplication(ESP_ZB_ZCL_AI_TEMPERATURE_OTHER);
  zbTemp1.setAnalogInputDescription("C");
  zbTemp1.setAnalogInputResolution(0.01);
  zbTemp1.setAnalogInputReporting(0, 10, 0.1);
  Zigbee.addEndpoint(&zbTemp1);

  Serial.println("Starting Zigbee...");

  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }

  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("Connected");
}

void loop() {

  float analog = (float)analogRead(temp1Pin);
  zbTemp1.setAnalogInput(analog);

  if (digitalRead(button) == LOW) {  // Push button pressed
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
  }

  delay(500);
}
