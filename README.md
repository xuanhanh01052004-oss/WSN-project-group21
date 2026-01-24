# WSN – Wireless Sensor Network for 100-Hectare Field Monitoring
Dự án xây dựng **mạng cảm biến không dây (WSN)** để giám sát **nhiệt độ – độ ẩm môi trường** (có thể mở rộng thêm độ ẩm đất) trên cánh đồng/vườn quy mô **~100 hecta**.  
Các node cảm biến thu thập dữ liệu theo chu kỳ và truyền về **Gateway**; dữ liệu được lưu trữ theo ngày và hiển thị trực quan.

---

## 1. Mục tiêu
- Thu thập dữ liệu nhiệt độ/độ ẩm **theo thời gian thực hoặc theo chu kỳ**.
- Truyền dữ liệu **không dây tầm xa** phù hợp diện tích lớn.
- Lưu trữ dữ liệu theo ngày, hỗ trợ truy xuất và thống kê.
- Dễ mở rộng số lượng node, dễ bảo trì và nâng cấp.

---

## 2. Kiến trúc hệ thống
Hệ thống gồm 3 lớp chính:

1) **Sensor Node (nhiều node)**
- Cảm biến: **DHT22** (nhiệt độ/độ ẩm)
- Vi điều khiển: **STM32** (ví dụ STM32F103)
- Kết nối không dây: **LoRa (SX1278/RA-01 433MHz)**

2) **Relay/Gateway**
- Vi điều khiển: **ESP32**
- Reley nhận gói tin từ các node được định danh ID và forward lên thăng gateway
- Module LoRa nhận/gửi gói tin với các node
- Chuyển tiếp dữ liệu từ client ESP32Gateway lên server qua WiFi (HTTP/REST)

3) **Server / Web**
- Nhận dữ liệu từ Gateway qua API
- Lưu log theo ngày (JSON/DB tùy triển khai)
- Giao diện web hiển thị dữ liệu (biểu đồ, bảng)

---

## 3. Luồng dữ liệu (Data Flow)
1. Node đọc DHT22 → tạo gói tin (ID node, T, H, timestamp, RSSI/SNR nếu có)  
2. Node gửi gói tin qua LoRa → Relay nhận
3. Relay fw -> Gateway
4. Gateway parse gói tin → gửi lên Server qua HTTP (JSON)  
5. Server lưu trữ dữ liệu theo ngày → Web hiển thị

---

## 4. Cấu trúc thư mục (Repository Structure)
- `sensornode`: https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/arduino/libraries/DHT22/sensor_2node/sensor_2node
- `ESP32 relay/`:(https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_Relay/ESP32_Relay)
- `ESP32 Gateway/`:(https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_Relay/ESP32_Relay)](https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_FULL/client/client_ESP32)
- `Server/`: https://github.com/xuanhanh01052004-oss/WSN-project/tree/master/esp32/ESP32_FULL/server


---

## 5. Giao thức gói tin (Packet Format)
Ví dụ payload dạng chuỗi/JSON (có thể thay đổi tùy triển khai):

- **Text**: `ID,T,H,BAT`
- **JSON**:
```json
{
  "node_id": "N01",
  "temp": 29.4,
  "humi": 71.2,
  "time": "2026-01-24T10:30:00",
  "rssi": -87
}
