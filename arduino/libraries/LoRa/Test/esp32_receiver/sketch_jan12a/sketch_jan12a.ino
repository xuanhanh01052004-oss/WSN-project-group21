#include <SPI.h>
#include <LoRa.h>

// --- Cấu hình chân cho STM32 (Blue Pill) ---
#define SS_PIN   PA4
#define RST_PIN  PB0
#define DIO0_PIN PB1

int counter = 0; // Biến đếm

void setup() {
  Serial.begin(115200);
  while (!Serial); // Chờ mở Serial Monitor (chỉ cần khi debug USB)

  Serial.println("STM32 LoRa Sender Test");

  // Cài đặt chân cho thư viện
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  
  // Khởi động LoRa ở 433MHz
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // Cài đặt Sync Word (Mật khẩu mạng) - Khớp với bên nhận
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initialized!");
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // --- Bắt đầu gửi ---
  LoRa.beginPacket();
  LoRa.print("Packet: ");
  LoRa.print(counter);
  LoRa.endPacket();
  // --- Kết thúc gửi ---

  counter++;
  delay(1000); // Gửi mỗi 1 giây
}