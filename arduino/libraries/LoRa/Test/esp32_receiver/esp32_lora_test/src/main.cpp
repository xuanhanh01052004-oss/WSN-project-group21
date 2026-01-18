#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

/*
  ESP32 DOIT DevKit V1 (VSPI default)
  SCK=18, MISO=19, MOSI=23
  Khuyến nghị: CS dùng GPIO27 để debug (tránh strap GPIO5)
*/

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23

#define LORA_SS    27   // <-- khuyến nghị dùng 27. Nếu bạn đang cắm CS=5 thì đổi lại 5
#define LORA_RST   14
#define LORA_DIO0  26   // bạn đang cắm DIO0=26

#define LORA_FREQ  433E6

SPIClass spiLoRa(VSPI);

// --- SPI read register (SX127x) ---
uint8_t sx_readReg(uint8_t addr) {
  spiLoRa.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(LORA_SS, LOW);
  delayMicroseconds(2);
  spiLoRa.transfer(addr & 0x7F);       // read
  uint8_t v = spiLoRa.transfer(0x00);
  digitalWrite(LORA_SS, HIGH);
  spiLoRa.endTransaction();
  return v;
}

void sx_reset() {
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(10);
  digitalWrite(LORA_RST, HIGH);
  delay(10);
}

bool sx_checkVersion() {
  // test MISO idle (bắt lỗi MISO bị kéo xuống)
  pinMode(LORA_MISO, INPUT_PULLUP);
  delay(5);
  int miso_idle = digitalRead(LORA_MISO);
  pinMode(LORA_MISO, INPUT);

  Serial.print("MISO idle = ");
  Serial.println(miso_idle); // 1 thường là bình thường, 0 là đáng nghi (kéo xuống)

  uint8_t opmode = sx_readReg(0x01);
  uint8_t ver    = sx_readReg(0x42);

  Serial.printf("RegOpMode(0x01)   = 0x%02X\n", opmode);
  Serial.printf("RegVersion(0x42)  = 0x%02X\n", ver);

  // SX1278/SX1276 family thường trả về 0x12 ở RegVersion
  return (ver == 0x12);
}

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("\nBOOT OK");

  // Pin setup
  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);

  // Start SPI bus
  spiLoRa.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

  // Hard reset chip
  sx_reset();

  Serial.println("== SX127x SPI check ==");
  if (!sx_checkVersion()) {
    Serial.println("\n[FAIL] Khong doc duoc RegVersion=0x12 => SPI/CS/MISO/nguon/RST dang sai.");
    Serial.println("Go y sua nhanh:");
    Serial.println("1) Kiem tra RA-01 cap 3.3V va GND chung");
    Serial.println("2) Kiem tra dung chan: SCK(18) MISO(19) MOSI(23) CS(27) RST(14)");
    Serial.println("3) Thu doi day MISO hoac doi module RA-01 khac");
    Serial.println("4) Neu ban dang cam CS vao GPIO5, hay doi sang GPIO27 (hoac nguoc lai) cho dung");
    while (true) delay(1000);
  }

  Serial.println("\n== LoRa init ==");
  LoRa.setSPI(spiLoRa);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("[FAIL] LoRa.begin() failed!");
    while (true) delay(1000);
  }

  // Profile test ổn định (2 bên phải giống nhau)
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  Serial.println("[OK] LoRa RX ready @433MHz");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String payload;
    payload.reserve(packetSize);

    while (LoRa.available()) {
      payload += (char)LoRa.read();
    }

    long rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    Serial.print("RX: ");
    Serial.print(payload);
    Serial.print(" | RSSI=");
    Serial.print(rssi);
    Serial.print(" dBm | SNR=");
    Serial.println(snr);
  }
}
