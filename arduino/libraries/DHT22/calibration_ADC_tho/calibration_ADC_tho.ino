// ===== Calibration: read raw ADC =====
// Sensor OUT -> A0 (PA0 on Bluepill), VCC -> 3.3V, GND -> GND

const int SoilPin = A0; // PA0 nếu bạn dùng Blue Pill

void setup() {
  Serial.begin(9600);
}

void loop() {
  int val = analogRead(SoilPin);
  Serial.println(val);
  delay(100);
}
