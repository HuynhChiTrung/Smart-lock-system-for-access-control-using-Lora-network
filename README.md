# 🔐 Hệ Thống Khóa Thông Minh Đa Phương Thức

## 🧩 Mô tả Dự Án

Hệ thống được thiết kế như một **ổ khóa thông minh**, tích hợp ba phương thức xác thực hiện đại gồm:

- ✅ Cảm biến vân tay (AS608)  
- 📶 Thẻ RFID (RC522)  
- 🔢 Bàn phím số cảm ứng (MPR121)  

Người dùng có thể linh hoạt lựa chọn bất kỳ phương thức nào để mở cửa khi có nhu cầu ra vào tòa nhà.
![image](https://github.com/user-attachments/assets/1f48d10d-1b0c-4f69-a9cf-78ef74de5a39)


## 🏗️ Kiến Trúc Hệ Thống

- **Node**: Thiết bị xử lý xác thực tại điểm truy cập, ghi nhận thông tin ra vào.
- **Gateway**: Nhận dữ liệu từ các Node thông qua giao tiếp **LoRa**, sau đó gửi dữ liệu lên server qua WiFi.

## 📡 Giao Tiếp Dữ Liệu

- **Node → Gateway**: Sử dụng sóng **LoRa** (SX1278) để truyền thông tin xác thực.
- **Gateway → Server**: Giao tiếp qua **WiFi + HTTP/Webserver** để đồng bộ và lưu trữ dữ liệu người dùng.

## 📅 Quản Lý Thông Tin Ra Vào

- Dữ liệu ra vào được phân loại và lưu trữ **tự động theo ngày**, dễ dàng tra cứu và quản lý.
- Khi có người dùng mới, nhân viên bảo vệ sử dụng **giao diện Webserver** để thêm dữ liệu.
- Gateway sẽ **tự động đồng bộ dữ liệu mới** từ server về tất cả các Node.

## ⚙️ Tính Năng Nổi Bật

- ✅ **Hỗ trợ cấu hình WiFi và LoRa động**: Không cần nạp lại firmware khi thay đổi mạng.
- 📡 **Truyền dữ liệu ổn định** với công nghệ **LoRa**.
- 🔄 **Đồng bộ dữ liệu người dùng** giữa Server ↔ Gateway ↔ Node.
- 🔐 **Độ bảo mật cao** với nhiều lựa chọn xác thực.
- 🌐 **Webserver tích hợp** để cấu hình và thêm dữ liệu từ trình duyệt.

## 🧠 Công Nghệ Sử Dụng

| Thành phần        | Mô tả                          |
|------------------|--------------------------------|
| ESP32            | Vi điều khiển chính (Node & Gateway) |
| LoRa SX1278      | Truyền dữ liệu không dây tầm xa |
| AS608            | Cảm biến vân tay               |
| RC522            | Đọc thẻ RFID                   |
| MPR121           | Bàn phím số cảm ứng            |
| Webserver        | Cấu hình WiFi và cập nhật dữ liệu |
| HTTP             | Giao tiếp với server           |

## 🎯 Mục Tiêu

Hệ thống được phát triển nhằm:

- Đảm bảo an toàn và bảo mật trong quản lý ra vào.
- Đem lại sự tiện nghi tối đa cho cư dân hoặc người dùng.
- Hỗ trợ quản lý tập trung, dễ mở rộng và dễ bảo trì.

---

> ✅ **Dự án được triển khai thực tế với khả năng hoạt động ổn định, phù hợp với các ứng dụng trong tòa nhà, văn phòng, nhà thông minh, v.v.**
