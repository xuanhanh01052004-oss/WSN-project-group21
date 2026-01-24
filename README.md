
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

2) **Gateway**
- Vi điều khiển: **ESP32**
- Module LoRa nhận/gửi gói tin với các node
- Chuyển tiếp dữ liệu lên server qua WiFi (HTTP/REST)

3) **Server / Web**
- Nhận dữ liệu từ Gateway qua API
- Lưu log theo ngày (JSON/DB tùy triển khai)
- Giao diện web hiển thị dữ liệu (biểu đồ, bảng)

---

## 3. Luồng dữ liệu (Data Flow)
1. Node đọc DHT22 → tạo gói tin (ID node, T, H, timestamp, RSSI/SNR nếu có)  
2. Node gửi gói tin qua LoRa → Gateway nhận  
3. Gateway parse gói tin → gửi lên Server qua HTTP (JSON)  
4. Server lưu trữ dữ liệu theo ngày → Web hiển thị

---

---

## 4. Giao thức gói tin (Packet Format)
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
````

---

## 6. Hướng dẫn chạy nhanh (Quick Start)

### 6.1 Sensor Node (STM32)

* Mở project `.ioc` bằng **STM32CubeMX** / **STM32CubeIDE**
* Generate code → Build
* Nạp firmware cho board STM32
* Kiểm tra UART debug (nếu có)

### 6.2 Gateway (ESP32)

* Mở project ESP32 (Arduino IDE hoặc PlatformIO)
* Cấu hình WiFi + endpoint server
* Flash vào ESP32
* Mở Serial Monitor để xem log nhận LoRa và gửi HTTP

### 6.3 Server/Web

* Chạy server (Node.js/Express hoặc tùy bạn)
* Kiểm tra endpoint nhận dữ liệu
* Mở web để xem dữ liệu hiển thị

---

## 7. Công cụ sử dụng

* STM32CubeMX / STM32CubeIDE (hoặc Keil)
* Arduino IDE / PlatformIO (ESP32)
* Node.js/Express (Server) *(nếu có)*
* Git/GitHub quản lý phiên bản

---

## 8. Kế hoạch mở rộng (Future Work)

* Thêm cảm biến **độ ẩm đất**, ánh sáng, pH...
* Tối ưu năng lượng: sleep mode, chu kỳ đo thích ứng
* Cơ chế ACK/Retry, chống mất gói
* Bảo mật: ký gói tin, mã hóa payload
* Dashboard nâng cao: biểu đồ theo ngày/tuần/tháng, cảnh báo ngưỡng

---
start).
```
