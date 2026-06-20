# Agro Monitor ESP32 (Smart Garden System)

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-lightgrey.svg)
![Framework](https://img.shields.io/badge/framework-Arduino-00979C.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**Agro Monitor** adalah sistem pemantauan cerdas (Smart Agriculture) berbasis mikrokontroler ESP32. Proyek ini dirancang untuk memantau parameter vital lingkungan tanaman secara real-time dengan antarmuka (UI) dashboard pada layar TFT 1.8" (ST7735) yang profesional.

---

## 📺 Antarmuka Dashboard Layar

Berikut adalah tampilan antarmuka (UI) pada layar TFT saat sistem beroperasi secara real-time:

<p align="center">
  <img src="<img width="287" height="396" alt="dashboard-ui" src="https://github.com/user-attachments/assets/eff7509a-7d00-4083-8709-98a1f758f2bc" />
" />
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
   #define CHAT_ID       "CHAT_ID_ANDA"![Uploading dashboard-ui.png…]()
