# Agro Monitor ESP32 (Smart Garden System) (ID)

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-lightgrey.svg)
![Framework](https://img.shields.io/badge/framework-Arduino-00979C.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**Agro Monitor** adalah sistem pemantauan cerdas (Smart Agriculture) berbasis mikrokontroler ESP32. Proyek ini dirancang untuk memantau parameter vital lingkungan tanaman secara real-time dengan antarmuka (UI) dashboard pada layar TFT 1.8" (ST7735) yang profesional.

---

## 📺 Antarmuka Dashboard Layar

Berikut adalah tampilan antarmuka (UI) pada layar TFT saat sistem beroperasi secara real-time:

<p align="center">
  <img width="287" height="396" alt="dashboard-ui" src="https://github.com/user-attachments/assets/962e20b8-a32c-47a2-b873-24a212fa1cd6" />
</p>

* **Mode Normal:** Menampilkan indikator angka secara langsung dengan status "SYS:OK".
* **Mode Peringatan (Pop-up):** Sistem akan menampilkan animasi peringatan layar penuh jika kadar air tanah berada di bawah 40% atau intensitas cahaya di bawah 200 lux.

---

## Fitur Utama

- **Real-Time Dashboard UI:** Visualisasi data Suhu, Kelembaban Udara, Kelembaban Tanah, dan Intensitas Cahaya dengan kepadatan piksel 128x160.
- **Sistem Peringatan Otomatis:** Animasi grafis peringatan kadar air/cahaya rendah yang muncul selama 4 detik, dilengkapi fitur pembatas interval (anti-spam) 5 menit.
- **Indikator LED Fisik:** - **Biru:** Kondisi lingkungan ideal / sistem berjalan normal.
  - **Merah:** Kondisi kritis (butuh tindakan) atau terjadi malfungsi pada sensor.
- **Hardware Fail-Safe:** Mampu mendeteksi jika ada kabel sensor yang terlepas atau rusak, dan menahan nilai error pada dashboard untuk mencegah data palsu.
- **Notifikasi Telegram (Khusus Online Mode):** Terintegrasi dengan bot Telegram untuk laporan terjadwal (NTP Sync) setiap jam 07:00 dan 17:00, serta peringatan darurat ke smartphone pengguna.

---

## Komponen Hardware

Untuk mengimplementasikan proyek ini, diperlukan komponen keras berikut:

* ESP32 Development Board (Mikrokontroler)
* Layar TFT 1.8" SPI (Driver ST7735, 7-Pin)
* Sensor DHT22 (Suhu & Kelembaban Udara)
* Capacitive Soil Moisture Sensor v1.2 (Kelembaban Tanah)
* Modul LDR Photoresistor (Intensitas Cahaya)
* 2x LED 5mm (Merah & Biru)
* 2x Resistor 220 Ohm (Untuk LED)
* Modul Step-Down LM2596 & PSU 12V (Untuk suplai daya eksternal)

---

## Konfigurasi Pin LCD TFT 1.8" SPI (7-Pin)

Konfigurasi koneksi pin antara ESP32 dengan modul layar TFT 7-Pin yang digunakan pada rilis ini adalah sebagai berikut:

| Pin LCD TFT 1.8" | Pin ESP32 | Fungsi / Keterangan |
| :--- | :--- | :--- |
| **VCC** | 3.3V / VIN | Jalur Catu Daya Modul |
| **GND** | GND | Ground Utama |
| **CS** | GPIO 5 (D5) | SPI Chip Select |
| **RESET / RST** | GPIO 4 (D4) | Reset Jalur Display |
| **A0 / DC** | GPIO 2 (D2) | Data / Command Selection |
| **SDA / MOSI** | GPIO 23 (D23) | SPI Master Output Slave Input |
| **SCK / SCLK** | GPIO 18 (D18) | SPI Clock Serial |

*Catatan: Pastikan pasokan arus pada jalur daya mencukupi, terutama jika modul layar yang digunakan menggabungkan pin Backlight (LED) langsung ke internal VCC.*

---

## Instalasi & Persiapan

### 1. Kebutuhan Library Arduino IDE
Pastikan Anda telah menginstal library berikut melalui **Library Manager** di Arduino IDE:
- `Adafruit GFX Library` by Adafruit
- `Adafruit ST7735 and ST7789 Library` by Adafruit
- `DHT sensor library` by Adafruit
- `UniversalTelegramBot` by Brian Lough (Khusus Online)
- `ArduinoJson` by Benoit Blanchon (Khusus Online)

### 2. Cara Penggunaan
1. **Pilih Mode:** Buka folder kode sumber dan pilih sketch sesuai kebutuhan Anda (`Agro_Monitor_Online.ino` atau `Agro_Monitor_Offline.ino`).
2. **Setup Kredensial (Khusus Online Mode):** Ubah baris berikut sesuai dengan jaringan WiFi dan Token Telegram Bot Anda:
   ```cpp
   #define WIFI_SSID     "Nama_WiFi_Anda"
   #define WIFI_PASSWORD "Password_WiFi"
   #define BOT_TOKEN     "TOKEN_DARI_BOTFATHER"
   #define CHAT_ID       "CHAT_ID_ANDA"

# Agro Monitor ESP32 (Smart Garden System) (EN)

[English](#english) | [Bahasa Indonesia](#bahasa-indonesia)

---

## English

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-lightgrey.svg)
![Framework](https://img.shields.io/badge/framework-Arduino-00979C.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**Agro Monitor** is a smart agriculture monitoring system based on the ESP32 microcontroller. This project is designed to monitor vital plant environmental parameters in real-time with a professional TFT 1.8" (ST7735) dashboard user interface.

### Dashboard Interface

Below is the real-time user interface (UI) displayed on the TFT screen:

![Dashboard Agro Monitor](https://github.com/user-attachments/assets/1a0d59bd-4b7d-44e1-9e41-5cf69e868c26.png)

* **Normal Mode:** Displays live numerical indicators with a "SYS:OK" status.
* **Warning Mode (Pop-up):** The system triggers a full-screen warning animation if the soil moisture drops below 40% or light intensity falls below 200 lux.

### Key Features

* **Real-Time Dashboard UI:** Data visualization for Temperature, Air Humidity, Soil Moisture, and Light Intensity packed in a 128x160 pixel density.
* **Automated Warning System:** Low water/light graphical animation that appears for 4 seconds, equipped with a 5-minute anti-spam interval limit.
* **Physical LED Indicators:** * **Blue:** Ideal environmental conditions / system running normally.
  * **Red:** Critical condition (action required) or sensor hardware malfunction.
* **Hardware Fail-Safe:** Capable of detecting disconnected or damaged sensor cables, holding error values on the dashboard to prevent false data readings.
* **Telegram Notification (Online Mode Only):** Integrated with a Telegram bot for scheduled reports (NTP Sync) every 07:00 and 17:00, alongside emergency alerts sent directly to the user's smartphone.

### Hardware Components

* ESP32 Development Board
* TFT 1.8" SPI Display (ST7735 Driver, 7-Pin)
* DHT22 Sensor (Temperature & Humidity)
* Capacitive Soil Moisture Sensor v1.2
* LDR Photoresistor Module
* 2x 5mm LEDs (Red & Blue)
* 2x 220 Ohm Resistors
* LM2596 Step-Down Module & 12V PSU (For external power supply)

### Pin Configuration (TFT 1.8" SPI)

| TFT 1.8" Pin | ESP32 Pin | Function / Description |
| :--- | :--- | :--- |
| **VCC** | 3.3V / VIN | Module Power Supply |
| **GND** | GND | Common Ground |
| **CS** | GPIO 5 (D5) | SPI Chip Select |
| **RESET / RST** | GPIO 4 (D4) | Display Reset |
| **A0 / DC** | GPIO 2 (D2) | Data / Command Selection |
| **SDA / MOSI** | GPIO 23 (D23) | SPI Master Output Slave Input |
| **SCK / SCLK** | GPIO 18 (D18) | SPI Serial Clock |

### Installation & Setup

1. Install the required libraries via the Arduino IDE Library Manager: `Adafruit GFX`, `Adafruit ST7735 and ST7789`, `DHT sensor library`, `UniversalTelegramBot`, and `ArduinoJson`.
2. Open the source code (`Agro_Monitor_Online.ino` or `Agro_Monitor_Offline.ino`).
3. If using the Online version, configure your WiFi and Telegram Bot credentials:
   ```cpp
   #define WIFI_SSID     "Your_WiFi_Name"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   #define BOT_TOKEN     "YOUR_BOTFATHER_TOKEN"
   #define CHAT_ID       "YOUR_CHAT_ID"
