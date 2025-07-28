#include "Zigbee.h"

const int fanPin1 = 1;
int bits = 10 ^ 2;

uint8_t analogPin = A0;
uint8_t button = BOOT_PIN;

ZigbeeAnalog zbFan1 = ZigbeeAnalog(1);

void onFan1OutputChange(float analog_output) {
  float percentage = analog_output * 0.01;
  ledcWrite(fanPin1, percentage * bits);
  Serial.printf("Received analog output change: %.1f\r\n", analog_output);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  ledcAttach(fanPin1, 500, 10);
  pinMode(button, INPUT_PULLUP);

  zbFan1.setManufacturerAndModel("Allan", "ZigbeeFan1");
  zbFan1.addAnalogOutput();
  zbFan1.setAnalogOutputApplication(ESP_ZB_ZCL_AI_PERCENTAGE_OTHER);
  zbFan1.setAnalogOutputDescription("Percentage");
  zbFan1.setAnalogOutputResolution(0.01);
  zbFan1.setAnalogOutputMinMax(0, 100);
  zbFan1.onAnalogOutputChange(onFan1OutputChange);
  Zigbee.addEndpoint(&zbFan1);

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

  delay(100);
}
