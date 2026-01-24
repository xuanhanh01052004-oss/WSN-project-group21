# WSN – Wireless Sensor Network for 100-Hectare Field Monitoring
Dự án xây dựng **mạng cảm biến không dây (WSN)** để giám sát **nhiệt độ – độ ẩm môi trường** (có thể mở rộng thêm **độ ẩm đất**) trên cánh đồng/vườn quy mô **~100 hecta**.  
Các node cảm biến thu thập dữ liệu theo chu kỳ và truyền về **Gateway**; dữ liệu được lưu trữ theo ngày và hiển thị trực quan trên web dashboard.

---

## 1. Mục tiêu
- Thu thập dữ liệu nhiệt độ/độ ẩm **theo thời gian thực hoặc theo chu kỳ**.
- Truyền dữ liệu **không dây tầm xa** phù hợp diện tích lớn.
- Lưu trữ dữ liệu theo ngày, hỗ trợ truy xuất và thống kê.
- Dễ mở rộng số lượng node, dễ bảo trì và nâng cấp.

---

## 2. Kiến trúc hệ thống
Hệ thống gồm 3 lớp chính:

### 2.1 Sensor Node (nhiều node)
- Cảm biến: **DHT22** (nhiệt độ/độ ẩm)
- (Tuỳ chọn) Cảm biến độ ẩm đất capacitive(đọc ADC)
- Vi điều khiển: **STM32** (ví dụ STM32F103C8T6)
- Kết nối không dây: **LoRa (SX1278/RA-01/02 433MHz)**

### 2.2 ESP32 Relay + ESP32 Gateway
- **ESP32 Relay**: nhận gói tin LoRa từ các node (có ID), sau đó **forward bằng LoRa 433MHz** lên Gateway để mở rộng vùng phủ.
- **ESP32 Gateway**: nhận dữ liệu (trực tiếp hoặc qua relay), parse gói tin, đọc RSSI/SNR và gửi dữ liệu lên server qua WiFi (HTTP/REST).

### 2.3 Server / Web
- Nhận dữ liệu từ Gateway qua API.
- Lưu log theo ngày (JSON/DB tùy triển khai).
- Giao diện web hiển thị dữ liệu (bảng, biểu đồ, realtime).

---

## 3. Luồng dữ liệu (Data Flow)
1. Node đọc DHT22 → tạo gói tin (ID node, T, H, timestamp, …).
2. Node gửi gói tin qua LoRa 433MHz → **ESP32 Relay** nhận.
3. Relay **forward** gói tin qua LoRa 433MHz → **ESP32 Gateway** nhận.
4. Gateway parse gói tin → gửi lên Server qua HTTP (JSON).
5. Server lưu trữ dữ liệu theo ngày → Web hiển thị.

---

## 4. Cấu trúc thư mục (Repository Structure)
> Link theo đúng folder trong repo hiện tại:

- **Sensor Node (Arduino/STM32 code)**  
  - `sensornode`: https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/arduino/libraries/DHT22/sensor_2node/sensor_2node

- **ESP32 Relay (LoRa 433MHz forward)**  
  - `ESP32_Relay`: https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_Relay/ESP32_Relay

- **ESP32 Gateway (LoRa 433MHz + WiFi uplink)**  
  - `client_ESP32`: https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_FULL/client/client_ESP32

- **Server (Node.js/Express + lưu log + API)**  
  - `server`: https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_FULL/server

---

## 5. Giao thức gói tin (Packet Format)
Hệ thống dùng payload dạng **key-value** phân tách bằng dấu `;` để dễ parse và debug.

### 5.1 Payload từ Node (STM32 → Relay/Gateway)
Ví dụ:
`ID=2;SEQ=31;T=26.4;H=71.0;ADC=512;SOIL=53;STATE=Wet`

Trong đó:
- `ID`: node id  
- `SEQ`: số thứ tự gói tin  
- `T`, `H`: nhiệt độ/độ ẩm  
- `ADC`, `SOIL`, `STATE`: thông tin độ ẩm đất (tuỳ chọn)

### 5.2 Payload forward của Relay (Relay → Gateway)
Relay thêm prefix để Gateway biết gói đi qua relay nào:
`TYPE=FWD;REL=10;` + payload gốc của Node

Ví dụ:
`TYPE=FWD;REL=10;ID=2;SEQ=31;T=26.4;H=71.0;ADC=512;SOIL=53;STATE=Wet`

---

## 6. Kiến trúc truyền thông (LoRa 433MHz)
Luồng tổng quát:

**STM32 Sensor Node (LoRa 433MHz)** → **ESP32 Relay (LoRa 433MHz)** → **ESP32 Gateway (LoRa 433MHz + WiFi)** → **Node.js Server** → **Web Dashboard**

- Node: đo DHT22 + (tuỳ chọn) độ ẩm đất (ADC), đóng gói dữ liệu và phát LoRa.
- Relay: nhận gói LoRa từ Node rồi **forward** lên Gateway (mở rộng vùng phủ).
- Gateway: nhận LoRa, đọc RSSI/SNR và POST dữ liệu lên server qua WiFi.
- Server: lưu theo ngày (JSON) và cung cấp REST API cho web hiển thị + export.

---

## 7. Tần số & cấu hình LoRa
Tần số: **433 MHz**.

Các tham số LoRa cần đồng bộ giữa Node / Relay / Gateway:
- SyncWord: `0x12`
- CRC: enable
- SF: 7
- BW: 125 kHz
- CR: 4/5

> (Nếu nhóm bạn dùng thông số khác, chỉ cần sửa đúng theo cấu hình thực tế.)

---

## 8. Backend (Node.js)
Server cung cấp các endpoint chính:
- `POST /api/uplink`: nhận `{ raw, rssi, snr }` từ Gateway và lưu log theo ngày
- `GET /api/data?node=<id>&limit=<n>`: trả dữ liệu hôm nay (lọc theo node, giới hạn số điểm)
- `GET /api/latest?node=<id>`: bản ghi mới nhất
- `GET /download-excel`: export CSV (mở bằng Excel)

Dữ liệu được lưu trong thư mục `data_logs/` theo từng ngày `YYYY-MM-DD.json`.

---

## 9. Web Dashboard
Trang `index.html` hiển thị:
- Thẻ realtime (Temp/Hum/Soil/State)
- Biểu đồ theo thời gian thực
- Bảng lịch sử gần nhất (tự refresh)
- Filter theo Node (All / Node 1 / Node 2 / Node 3)
- Nút export CSV

---

## 10. Chạy demo nhanh
### 10.1 Nạp firmware
1) Flash cho:
- STM32 Node
- ESP32 Relay
- ESP32 Gateway

2) Đảm bảo Node/Relay/Gateway dùng **cùng cấu hình LoRa 433MHz** (SF/BW/CR/SyncWord).

### 10.2 Chạy server
```bash
node server.js
10.3  Mở web

http://localhost:3000/ (trên máy chạy server)

hoặc http://<IP_laptop>:3000/ (thiết bị khác cùng mạng)

11. Hướng phát triển (Future Work)

Thêm cơ chế ACK/retry end-to-end để giảm mất gói

Mã hóa payload (AES) + chống giả mạo

Tối ưu duty-cycle để tăng tuổi thọ pin (sleep mode)

Nhiều relay/gateway và routing đơn giản (giới hạn hop, chống loop)

OTA cho ESP32

Dashboard nâng cao: thống kê theo ngày/tuần/tháng, cảnh báo vượt ngưỡng
