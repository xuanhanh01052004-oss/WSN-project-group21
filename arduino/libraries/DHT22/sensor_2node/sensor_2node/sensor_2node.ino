#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>

// ====== DHT22 ======
#define DHTPIN  PA2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ====== Soil sensor ======
const int SoilPin    = A0;   // PA0
const int AirValue   = 660;  // ADC khi khô
const int WaterValue = 197;  // ADC khi nhúng nước
int intervals;

int clampADC(int adc) {
  if (adc > AirValue) return AirValue;
  if (adc < WaterValue) return WaterValue;
  return adc;
}

int soilPercentFromADC(int adc) {
  adc = clampADC(adc);
  long pct = (long)(AirValue - adc) * 100L / (AirValue - WaterValue);
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return (int)pct;
}

const char* soilStateFromADC(int adc) {
  adc = clampADC(adc);
  if (adc >= WaterValue && adc < (WaterValue + intervals)) return "Very Wet";
  if (adc >= (WaterValue + intervals) && adc < (AirValue - intervals)) return "Wet";
  return "Dry";
}

// ====== LoRa pins (Blue Pill SPI1) ======
#define LORA_SS   PA4
#define LORA_RST  PB0   // CÁCH 1: đổi RST sang PB0
#define LORA_DIO0 PA1

static uint16_t seq = 0;
static const uint8_t NODE_ID = 1; // bạn đổi 1/2/3 tùy node
//static const uint8_t NODE_ID = 2;
void setup() {
  Serial.begin(9600);
  delay(200);

  dht.begin();
  intervals = (AirValue - WaterValue) / 3;

  // ---- LoRa init (chỉ thêm) ----
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1) delay(1000);
  }

  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  Serial.println("LoRa init OK (433MHz)");

  Serial.println("=== STM32 + DHT22 + Soil Capacitive v1.2 ===");
  Serial.println("Format: TempC | HumAir% | ADCsoil | Soil% | State");
}

void loop() {
  // ---- Read DHT22 ----
  float humAir = dht.readHumidity();
  float tempC  = dht.readTemperature();

  // ---- Read Soil ADC ----
  int adc = analogRead(SoilPin);
  int soilPct = soilPercentFromADC(adc);
  const char* state = soilStateFromADC(adc);

  // ---- Print (giữ nguyên) ----
  if (isnan(humAir) || isnan(tempC)) {
    Serial.print("DHT22 ERROR | ");
  } else {
    Serial.print("Nhiet do = ");
    Serial.print(tempC, 1);
    Serial.print(" C | Do am KK = ");
    Serial.print(humAir, 1);
    Serial.print(" % | ");
  }

  Serial.print("ADC soil = ");
  Serial.print(adc);
  Serial.print(" | Soil = ");
  Serial.print(soilPct);
  Serial.print(" % | ");
  Serial.println(state);

  // ---- THÊM: gửi LoRa payload ----
  char payload[120];
char tStr[12], hStr[12];

if (isnan(humAir) || isnan(tempC)) {
  snprintf(payload, sizeof(payload),
           "ID=%u;SEQ=%u;ADC=%d;SOIL=%d;STATE=%s",
           NODE_ID, seq++, adc, soilPct, state);
} else {
  // Chuyển float -> string (tránh snprintf %.1f bị rỗng)
  dtostrf(tempC, 0, 1, tStr);   // 1 chữ số thập phân
  dtostrf(humAir, 0, 1, hStr);

  snprintf(payload, sizeof(payload),
           "ID=%u;SEQ=%u;T=%s;H=%s;ADC=%d;SOIL=%d;STATE=%s",
           NODE_ID, seq++, tStr, hStr, adc, soilPct, state);
}


  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket();

  Serial.print("LoRa TX: ");
  Serial.println(payload);

  delay(5000); //
  //delay(6500);
}
