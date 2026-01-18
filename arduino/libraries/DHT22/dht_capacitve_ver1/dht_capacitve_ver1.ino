/*
#include <DHT.h>


#define DHTPIN PA2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#define SOIL_PIN PA0   // ADC

void setup() {
  Serial.begin(9600);
  delay(1000);

  dht.begin();

  Serial.println("=== STM32 + DHT22 + Soil Capacitive v1.2 ===");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  int adc = analogRead(SOIL_PIN);  // 0..4095 (12-bit)
  
  // Chuyển ADC -> % độ ẩm đất (cần calibrate)
  // dryValue: giá trị khi đất khô, wetValue: giá trị khi nhúng nước
  int dryValue = 3200;   // bạn chỉnh lại theo thực tế
  int wetValue = 1500;   // bạn chỉnh lại theo thực tế

  int soilPercent = map(adc, dryValue, wetValue, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  if (isnan(h) || isnan(t)) {
    Serial.println("Loi doc DHT22!");
  } else {
    Serial.print("Nhiet do: ");
    Serial.print(t);
    Serial.print(" C   | Do am khong khi: ");
    Serial.print(h);
    Serial.print(" %   | Do am dat: ");
    Serial.print(soilPercent);
    Serial.print(" %   (ADC=");
    Serial.print(adc);
    Serial.println(")");
  }

  delay(2000); // DHT22 nên đọc >=2s
}
*/
#include <DHT.h>

// ====== CHÂN KẾT NỐI ======
#define DHTPIN   PA2
#define DHTTYPE  DHT22
#define SOIL_PIN PA0   // ADC1_IN0

DHT dht(DHTPIN, DHTTYPE);

// ====== HIỆU CHUẨN (CALIBRATION) ======
// Bước 1: để sensor ngoài không khí/đất khô -> đọc ADC -> điền vào dryValue
// Bước 2: nhúng phần đo vào nước/đất bão hòa -> đọc ADC -> điền vào wetValue
// Quy luật thường gặp: khô ADC cao, ướt ADC thấp
int dryValue = 3200;   // <-- sửa theo ADC thực tế khi KHÔ
int wetValue = 1500;   // <-- sửa theo ADC thực tế khi ƯỚT

void setup() {
  Serial.begin(9600);
  delay(1000);

  dht.begin();

  Serial.println("=== STM32F103C8T6 + DHT22 + Soil Capacitive v1.2 ===");
  Serial.println("Luu y: Cap Soil sensor bang 3.3V (khong cap 5V)!");
  Serial.print("dryValue = "); Serial.println(dryValue);
  Serial.print("wetValue = "); Serial.println(wetValue);
  Serial.println("-----------------------------------------------");
}

void loop() {
  // ====== ĐỌC DHT22 ======
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  // ====== ĐỌC ĐỘ ẨM ĐẤT (ADC) ======
  int adc = analogRead(SOIL_PIN);  // 0..4095

  // Tính %: 0% = khô, 100% = ướt
  // soil% = (dry - adc) / (dry - wet) * 100
  int soilPercent = (int)((float)(dryValue - adc) * 100.0f / (float)(dryValue - wetValue));
  soilPercent = constrain(soilPercent, 0, 100);

  // ====== IN RA SERIAL ======
  Serial.print("ADC soil = ");
  Serial.print(adc);

  Serial.print(" | Soil = ");
  Serial.print(soilPercent);
  Serial.print(" %");

  Serial.print(" | ");

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT22: Loi doc!");
  } else {
    Serial.print("Nhiet do = ");
    Serial.print(t, 1);
    Serial.print(" C");

    Serial.print(" | Do am khong khi = ");
    Serial.print(h, 1);
    Serial.println(" %");
  }

  delay(2000); // DHT22 khuyến nghị đọc >= 2 giây
}
