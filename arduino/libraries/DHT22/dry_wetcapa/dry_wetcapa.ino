/*
const int AirValue = 650; //you need to change this value that you had recordedintheair
const int WaterValue = 260; //you need to change this value that you had recordedinthewaterint intervals = (AirValue - WaterValue)/3;
int soilMoistureValue = 0;
void setup() {
Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
}
void loop() {
soilMoistureValue = analogRead(A0); //put Sensor insert into soil
if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals)){
Serial.println("Very Wet");
}
else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue-intervals)){
Serial.println("Wet");
}
else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals)){
Serial.println("Dry");
}
delay(1000);
}
*/

const int SoilPin    = A0;   // với STM32 có thể dùng PA0, nhưng A0 thường vẫn đúng
const int AirValue   = 650;  // ADC khi khô/không khí
const int WaterValue = 197;  // ADC khi nhúng nước

int intervals;

int soilPercentFromADC(int adc) {
  // 0% tại AirValue, 100% tại WaterValue (càng ẩm ADC càng nhỏ)
  long pct = (long)(AirValue - adc) * 100L / (AirValue - WaterValue);
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return (int)pct;
}

void setup() {
  Serial.begin(9600);
  intervals = (AirValue - WaterValue) / 3;
}

void loop() {
  int adc  = analogRead(SoilPin);
  int soil = soilPercentFromADC(adc);

  // Xác định trạng thái
  const char* state = "Out of range";
  if (adc >= WaterValue && adc < (WaterValue + intervals)) {
    state = "Very Wet";
  } else if (adc >= (WaterValue + intervals) && adc < (AirValue - intervals)) {
    state = "Wet";
  } else if (adc >= (AirValue - intervals) && adc <= AirValue) {
    state = "Dry";
  }

  // In ra đầy đủ
  Serial.print("ADC=");
  Serial.print(adc);
  Serial.print(" | Soil=");
  Serial.print(soil);
  Serial.print("% | ");
  Serial.println(state);

  delay(1000);
}
