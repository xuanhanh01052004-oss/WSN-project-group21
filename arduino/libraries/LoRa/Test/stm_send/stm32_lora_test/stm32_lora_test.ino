#include <SPI.h>
#include <LoRa.h>

// Blue Pill SPI1: SCK=PA5 MISO=PA6 MOSI=PA7
// NSS=PA4, RST=PA0, DIO0=PA1
#define LORA_SS   PA4
#define LORA_RST  PA0
#define LORA_DIO0 PA1

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Một số core STM32 cần set chân SPI explicit:
  SPI.setMOSI(PA7);
  SPI.setMISO(PA6);
  SPI.setSCLK(PA5);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1) delay(1000);
  }

  // Cấu hình phải giống ESP32
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  Serial.println("STM32 LoRa TX ready");
}

void loop() {
  static uint32_t cnt = 0;
  String msg = "HELLO from STM32 cnt=" + String(cnt++);

  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();          // gửi xong mới return

  Serial.println("TX: " + msg);
  delay(1000);
}
