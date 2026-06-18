# Agro Monitor ESP32 (Smart Garden System)

![Version](https://img.shields.io/badge/version-2.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-lightgrey.svg)
![Framework](https://img.shields.io/badge/framework-Arduino-00979C.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**Agro Monitor** adalah sistem pemantauan cerdas (Smart Agriculture) berbasis mikrokontroler ESP32. Proyek ini dirancang untuk memantau parameter vital lingkungan tanaman secara real-time dengan antarmuka (UI) dashboard TFT yang profesional.

Proyek ini menyediakan dua mode operasional (keduanya tersedia dalam repositori ini):
1. **Online Mode:** Terintegrasi dengan WiFi, sinkronisasi waktu NTP, dan notifikasi otomatis via Telegram Bot.
2. **Offline Mode:** Mode standalone hemat daya tanpa memerlukan koneksi internet, cocok untuk area perkebunan terpencil.

---

## Fitur Utama

### Fitur Umum (Tersedia di Kedua Mode)
- **Real-Time Dashboard UI:** Menampilkan data Suhu, Kelembaban Udara, Kelembaban Tanah, dan Intensitas Cahaya secara bersamaan pada layar TFT 1.8" (128x160).
- **Smart Pop-up Warning:** Sistem animasi peringatan (ikon tetesan air) yang otomatis muncul menimpa layar utama selama 4 detik saat kondisi tanah kering (<40%) atau kurang cahaya (<200 lux).
- **Anti-Spam Alert:** Notifikasi peringatan dibatasi agar hanya muncul 1 kali setiap 5 menit jika kondisi kritis belum ditangani.
- **Indikator LED Fisik:**
  - **Biru:** Kondisi lingkungan ideal / aman.
  - **Merah:** Kondisi kritis (butuh tindakan) atau terjadi malfungsi pada sensor (hardware error).
- **Hardware Fail-Safe:** Mampu mendeteksi jika ada kabel sensor yang terlepas atau rusak, mengganti angka dengan indikator `-` pada layar untuk mencegah pembacaan data palsu.

### Khusus Online Mode
- **Integrasi Telegram Bot:** Ketik `/ceksuhu` di Telegram untuk mendapatkan laporan status tanaman saat itu juga secara instan.
- **Laporan Terjadwal (NTP Sync):** Bot akan otomatis mengirimkan ringkasan status kebun setiap jam 07:00 pagi dan 17:00 sore.
- **Alarm Otomatis:** Bot akan mengirimkan pesan darurat ke Telegram jika sistem mendeteksi tanah mengering atau cahaya terlalu redup.

---

## Komponen Hardware

Untuk mengimplementasikan proyek ini, diperlukan komponen keras berikut:

* ESP32 Development Board (Mikrokontroler)
* Layar TFT 1.8" SPI (Driver ST7735)
* Sensor DHT22 (Suhu & Kelembaban Udara)
* Capacitive Soil Moisture Sensor v1.2 (Kelembaban Tanah)
* Modul LDR Photoresistor (Intensitas Cahaya)
* 2x LED 5mm (Merah & Biru)
* 1x Resistor 1k Ohm (Untuk LED Merah)
* Breadboard & Kabel Jumper

---

## Konfigurasi Pin (Wiring Diagram)

Pastikan sambungan kabel (wiring) sesuai dengan tabel berikut:

| Komponen | Pin Modul | Pin ESP32 | Keterangan |
| :--- | :--- | :--- | :--- |
| **TFT ST7735** | VCC & BL | 3.3V | Daya & Backlight Layar |
| | GND | GND | Ground |
| | CS | D5 (GPIO 5) | SPI Chip Select |
| | RST | D4 (GPIO 4) | Reset Layar |
| | A0 / DC | D2 (GPIO 2) | Data/Command |
| | SDA / MOSI | D23 (GPIO 23)| SPI Data |
| | SCK / SCLK | D18 (GPIO 18)| SPI Clock |
| **DHT22** | DATA (OUT) | D15 (GPIO 15)| Sensor Suhu & Humid |
| **Soil Moisture**| AOUT | D34 (ADC) | Sensor Tanah (Input Analog) |
| **LDR Sensor** | AO | D35 (ADC) | Sensor Cahaya (Input Analog) |
| **LED Indikator**| LED Merah (+) | D12 (GPIO 12)| Via Resistor 1k Ohm |
| | LED Biru (+) | D14 (GPIO 14)| |

---

## Instalasi & Persiapan

### 1. Kebutuhan Library Arduino IDE
Pastikan Anda telah menginstal library berikut melalui **Library Manager** di Arduino IDE:
- `Adafruit GFX Library` by Adafruit
- `Adafruit ST7735 and ST7789 Library` by Adafruit
- `DHT sensor library` by Adafruit
- `UniversalTelegramBot` by Brian Lough
- `ArduinoJson` by Benoit Blanchon (Wajib untuk Telegram)

### 2. Cara Penggunaan
1. **Pilih Mode:** Buka folder kode sumber dan pilih sketch sesuai kebutuhan Anda (`Agro_Monitor_Online.ino` atau `Agro_Monitor_Offline.ino`).
2. **Setup Kredensial (Khusus Online):** Ubah baris berikut sesuai dengan jaringan WiFi dan Token Telegram Bot Anda:
   ```cpp
   #define WIFI_SSID     "Nama_WiFi_Anda"
   #define WIFI_PASSWORD "Password_WiFi"
   #define BOT_TOKEN     "TOKEN_DARI_BOTFATHER"
   #define CHAT_ID       "CHAT_ID_ANDA"
