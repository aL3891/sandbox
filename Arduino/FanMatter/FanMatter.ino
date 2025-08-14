#include <Matter.h>
#include <MatterFan.h>

const int fanPin1 = 1;
int bits = 10 ^ 2;

MatterFan matter_fan;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  Matter.begin();
  matter_fan.begin();

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("Matter device is not commissioned");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
  }
  while (!Matter.isDeviceCommissioned()) {
    delay(200);
  }

  Serial.println("Waiting for Thread network...");
  while (!Matter.isDeviceThreadConnected()) {
    delay(200);
    decommission_handler();
  }
  Serial.println("Connected to Thread network");

  Serial.println("Waiting for Matter device discovery...");
  while (!matter_fan.is_online()) {
    delay(200);
    decommission_handler();
  }
  Serial.println("Matter device is now online");

  pinMode(fanPin1, OUTPUT);
  analogWriteResolution(10);
  //analogWriteFrequency(fanPin1, 500);  //25khz?
}

void loop() {

  decommission_handler();
  static uint8_t fan_last_speed = 0;
  uint8_t fan_current_speed = matter_fan.get_percent();

  if (fan_current_speed != fan_last_speed) {
    fan_last_speed = fan_current_speed;
    Serial.print("Fan speed: ");
    Serial.print(fan_current_speed);
    Serial.println("%");
    int speed = map(fan_current_speed, 0, 100, 0, 255);
    analogWrite(fanPin1, speed);
  }



  static bool fan_last_state = false;
  bool fan_current_state = matter_fan.get_onoff();

  if (fan_current_state != fan_last_state) {
    fan_last_state = fan_current_state;
    if (fan_current_state) {
      Serial.println("Fan ON");
    } else {
      Serial.println("Fan OFF");
    }
  }


  for (int x = 0; x < 100; x++) {
    onFan1OutputChange(x);
  }

  for (int x = 100; x > 0; x--) {
    onFan1OutputChange(x);
  }

  delay(100);
}

void decommission_handler()
{
  // If the button is not pressed or the device is not commissioned - return
  if (digitalRead(BTN_BUILTIN) != LOW || !Matter.isDeviceCommissioned()) {
    return;
  }

  // Store the time when the button was first pressed
  uint32_t start_time = millis();
  // While the button is being pressed
  while (digitalRead(BTN_BUILTIN) == LOW) {
    // Calculate the elapsed time
    uint32_t elapsed_time = millis() - start_time;
    // If the button has been pressed for less than 10 seconds, continue
    if (elapsed_time < 10000u) {
      yield();
      continue;
    }

    // Blink the LED to indicate the start of the decommissioning process
    for (uint8_t i = 0u; i < 10u; i++) {
      digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));
      delay(100);
    }

    Serial.println("Starting decommissioning process, device will reboot...");
    Serial.println();
    digitalWrite(LED_BUILTIN, LED_BUILTIN_INACTIVE);
    // This function will not return
    // The device will restart once decommissioning has finished
    Matter.decommission();
  }
}


void onFan1OutputChange(float analog_output) {
  float percentage = analog_output * 0.01;
  analogWrite(fanPin1, percentage * bits);
  Serial.printf("Received analog output change: %.1f\r\n", analog_output);
}
