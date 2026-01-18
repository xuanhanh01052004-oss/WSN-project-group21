#include <SPI.h>
#include <LoRa.h>

// --- Cấu hình chân cho ESP32 ---
#define SS_PIN   5
#define RST_PIN  14
#define DIO0_PIN 26

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("ESP32 LoRa Receiver Test");

  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // Sync Word phải GIỐNG HỆT bên gửi
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Receiver Ready!");
}

void loop() {
  // Kiểm tra gói tin đến
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    // Có tin mới!
    Serial.print("Received '");

    // Đọc nội dung tin nhắn
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }
    
    // In ra cường độ tín hiệu (RSSI)
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}