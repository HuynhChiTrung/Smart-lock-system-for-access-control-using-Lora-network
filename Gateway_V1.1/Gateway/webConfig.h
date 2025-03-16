
#ifndef WEBCONFIG_H
#define WEBCONFIG_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>Thiết lập WiFi</title>
    <meta charset="UTF-8">
    <style>
      body {
        font-family: Arial, sans-serif;
        background-color: #f0f0f0;
        margin: 0; padding: 20px;
      }
      h2 { color: #4CAF50; text-align: center; }
      form {
        display: flex;              /* Sử dụng flexbox trong form */
        flex-direction: column;     /* Căn giữa các phần tử theo chiều dọc */
        align-items: center;        /* Căn giữa các phần tử trong form */
        background-color: #fff; border-radius: 8px;
        padding: 20px; box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        max-width: 400px; margin: auto;
      }
      input[type="text"], input[type="password"] {
        width: 100%; padding: 10px; margin: 10px 0;
        border: 1px solid #ccc; border-radius: 4px;
      }
      button {
        background-color: #4CAF50; color: white;
        padding: 10px 15px; border: none; border-radius: 4px;
        cursor: pointer; width: 100%; margin-top: 10px;
        transition: background-color 0.3s;
      }
      button:active { background-color: #3e8e41; }
      #wifiList, #message {
        margin-top: 20px; padding: 10px; background-color: #fff;
        border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1);
      }
      #message { display: none; text-align: center; color: #4CAF50; }
      .wifi-item {
        cursor: pointer; padding: 5px 0;
      }
      .wifi-item:hover {
        background-color: #e0e0e0;
      }
    </style>
  </head>
  <body>
    <h2>Thiết lập WiFi</h2>
    <form id="wifiForm">
      Tên WiFi: <input type="text" name="ssid" placeholder="Nhập tên WiFi"><br>
      Mật khẩu: <input type="password" name="password" placeholder="Nhập mật khẩu"><br>
      <button type="button" onclick="saveWiFi()">LƯU</button>
    </form>
    <div id="message">Đã lưu thành công!</div>

    <div id="wifiList"></div>

    <script>
      // Lưu thông tin WiFi
      function saveWiFi() {
        const form = document.getElementById('wifiForm');
        const formData = new FormData(form);
        fetch('/save', { method: 'POST', body: formData })
          .then(response => response.text())
          .then(() => {
            document.getElementById('message').style.display = 'block';
            setTimeout(() => {
              document.getElementById('message').style.display = 'none';
            }, 3000); // Thời gian hiển thị thông báo trước khi chuyển hướng
          });
      }

    </script>
  </body>
</html>
)rawliteral";

#endif  // WEBCONFIG_H
