const int fanPin1 = 1;
int bits = 10 ^ 2;

void onFan1OutputChange(float analog_output) {
  float percentage = analog_output * 0.01;
  ledcWrite(fanPin1, percentage * bits);
  Serial.printf("Received analog output change: %.1f\r\n", analog_output);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(fanPin1, OUTPUT);
  analogWriteResolution(fanPin1, 10);
  analogWriteFrequency(fanPin1, 500); //25khz?
}

void loop() {

  for (int x = 0; x < 100; x++) {
    onFan1OutputChange(x);
  }

  for (int x = 100; x > 0; x--) {
    onFan1OutputChange(x);
  }

  delay(100);
}
