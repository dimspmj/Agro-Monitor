#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <time.h> // Library bawaan buat narik jam dari internet

// --- KREDENSIAL WIFI & BOT ---
#define WIFI_SSID     "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASS"
#define BOT_TOKEN     "BOT_TOKEN"
#define CHAT_ID       "BOT_ID"

// --- PIN HARDWARE ---
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
#define DHTPIN     15
#define DHTTYPE    DHT22 
#define SOIL_PIN   34
#define LDR_PIN    35
#define LED_MERAH  12
#define LED_BIRU   14

// --- WARNA DASHBOARD UTAMA ---
#define C_BG       0x0821  
#define C_ORANGE   0xFC00  
#define C_CYAN     0x04FF  
#define C_GREEN    0x07E0  
#define C_YELLOW   0xFFE0  
#define C_TEXT     0x7BEF  

// --- WARNA UI TELEGRAM & WARNING DROP ---
#define COLOR_BG       0x0000 
#define COLOR_HEADER   0x2104 
#define COLOR_MUTED    0x7BEF
#define COLOR_GREEN    0x07E0
#define COLOR_GREEN_DK 0x03E0
#define COLOR_RED      0xF800
#define COLOR_RED_DK   0x8000
#define C_AMBER        0xFD20
#define C_AMBER_DK     0xA100

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Kalibrasi Tanah
const int airKering = 3200; 
const int airBasah  = 1500;

// Variabel Penanda Jam Biar Gak Ngirim Berkali-kali di Menit yang Sama
bool reportPagiTerkirim = false;
bool reportSoreTerkirim = false;

// ============================================================
// FUNGSI UI HELPER & TELEGRAM FEEDBACK
// ============================================================
void drawHeader(const char* title, uint16_t color) {
  tft.fillRect(0, 0, 128, 18, COLOR_HEADER);
  tft.setTextColor(color); tft.setTextSize(1);
  int16_t x = (128 - strlen(title) * 6) / 2;
  tft.setCursor(x, 6); tft.print(title);
}

void drawFooter(const char* status, uint16_t color) {
  tft.fillRect(0, 148, 128, 12, COLOR_HEADER);
  tft.setTextColor(color); tft.setTextSize(1);
  int16_t x = (128 - strlen(status) * 6) / 2;
  tft.setCursor(x, 150); tft.print(status);
}

void drawTimestamp(const char* ts) {
  tft.setTextColor(COLOR_MUTED); tft.setTextSize(1);
  tft.setCursor(68, 140); tft.print(ts);
}

void drawCentered(const char* text, int16_t y, uint16_t color, uint8_t size = 1) {
  tft.setTextSize(size);
  int16_t x = (128 - strlen(text) * 6 * size) / 2;
  tft.setTextColor(color); tft.setCursor(x, y); tft.print(text);
}

void drawCheckIcon(int16_t cx, int16_t cy, int16_t r, uint16_t color) {
  tft.drawCircle(cx, cy, r, color); tft.drawCircle(cx, cy, r - 1, color);
  tft.drawLine(cx - 10, cy, cx - 3, cy + 8, color);
  tft.drawLine(cx - 3, cy + 8, cx + 10, cy - 8, color);
  tft.drawLine(cx - 10, cy + 1, cx - 3, cy + 9, color);
  tft.drawLine(cx - 3, cy + 9, cx + 10, cy - 7, color);
}

void drawCrossIcon(int16_t cx, int16_t cy, int16_t r, uint16_t color) {
  tft.drawCircle(cx, cy, r, color); tft.drawCircle(cx, cy, r - 1, color);
  tft.drawLine(cx - 9, cy - 9, cx + 9, cy + 9, color);
  tft.drawLine(cx + 9, cy - 9, cx - 9, cy + 9, color);
  tft.drawLine(cx - 8, cy - 9, cx + 9, cy + 8, color);
  tft.drawLine(cx + 9, cy - 8, cx - 8, cy + 9, color);
}

void drawWarningIcon(int16_t cx, int16_t ty, uint16_t color) {
  tft.drawTriangle(cx, ty, cx - 22, ty + 38, cx + 22, ty + 38, color);
  tft.drawTriangle(cx, ty + 1, cx - 21, ty + 38, cx + 21, ty + 38, color);
  tft.fillRect(cx - 1, ty + 10, 3, 16, color);
  tft.fillRect(cx - 1, ty + 29, 3, 4, color);
}

void drawErrorBox(int16_t y, const char* cause, const char* errcode, const char* hint) {
  tft.drawRect(6, y, 116, 42, COLOR_RED_DK);
  tft.setTextColor(0xD000); tft.setTextSize(1);
  tft.setCursor(12, y + 6); tft.print("PENYEBAB:");
  tft.setTextColor(COLOR_RED); tft.setCursor(12, y + 16); tft.print(cause);
  tft.setTextColor(COLOR_RED_DK); tft.setCursor(12, y + 26); tft.print(errcode);
  tft.setCursor(12, y + 36); tft.print(hint);
}

void showSuccess(const char* chatId, const char* ts) {
  tft.fillScreen(COLOR_BG); drawHeader("TELEGRAM BOT", COLOR_GREEN_DK);
  tft.fillCircle(64, 68, 26, 0x0200); drawCheckIcon(64, 68, 26, COLOR_GREEN);
  drawCentered("MESSAGE SENT", 104, COLOR_GREEN); drawCentered(chatId, 116, COLOR_GREEN_DK);
  tft.drawLine(6, 132, 122, 132, 0x0300);
  tft.setTextColor(0x0480); tft.setTextSize(1); tft.setCursor(10, 138); tft.print("HTTP 200");
  drawTimestamp(ts); drawFooter("STATUS: OK", COLOR_GREEN_DK);
}

void showFailedNoWifi(const char* ts) {
  tft.fillScreen(COLOR_BG); drawHeader("TELEGRAM BOT", COLOR_RED_DK);
  tft.fillCircle(64, 62, 24, 0x2000); drawCrossIcon(64, 62, 24, COLOR_RED);
  drawCentered("SEND FAILED", 96, COLOR_RED);
  drawErrorBox(106, "WiFi not connected", "ERR: NO_NETWORK", "Cek SSID/password");
  drawFooter("STATUS: ERROR", COLOR_RED_DK);
}

// ============================================================
// FUNGSI UI WARNING TANAH (DROP ICON)
// ============================================================
void drawSoilBar(int pct, uint16_t fillCol) {
  tft.setTextColor(COLOR_MUTED); tft.setTextSize(1);
  tft.setCursor((128 - 10 * 6) / 2, 108); tft.print("KADAR AIR");
  tft.drawRoundRect(12, 116, 104, 10, 4, COLOR_MUTED);
  int fillW = (int)(pct / 100.0 * 100);
  if (fillW > 0) tft.fillRoundRect(13, 117, fillW, 8, 3, fillCol);
  char buf[6]; snprintf(buf, sizeof(buf), "%d%%", pct);
  tft.setTextColor(fillCol); tft.setCursor(14, 129); tft.print(buf);
  tft.setTextColor(COLOR_MUTED); tft.setCursor(108, 129); tft.print("100");
}

void drawDrop(int16_t cx, int16_t ty, uint8_t level, uint16_t col) {
  tft.fillTriangle(cx, ty, cx - 14, ty + 24, cx + 14, ty + 24, (level == 0) ? (uint16_t)0x1000 : (uint16_t)0x0820);
  tft.fillCircle(cx, ty + 24, 14, (level == 0) ? (uint16_t)0x1000 : (uint16_t)0x0820);
  if (level == 1) {
    tft.fillRect(cx - 13, ty + 24, 27, 8, C_AMBER); tft.fillCircle(cx, ty + 24, 13, C_AMBER);
    tft.fillRect(cx - 13, ty + 16, 27, 8, 0x0820);
  } else if (level == 2) {
    tft.fillTriangle(cx, ty + 4, cx - 13, ty + 24, cx + 13, ty + 24, col);
    tft.fillCircle(cx, ty + 24, 13, col);
  }
  tft.drawTriangle(cx, ty, cx - 14, ty + 24, cx + 14, ty + 24, col); tft.drawCircle(cx, ty + 24, 14, col);
  if (level == 0) {
    tft.drawLine(cx, ty + 8, cx - 4, ty + 18, col); tft.drawLine(cx - 4, ty + 18, cx + 2, ty + 26, col);
    tft.fillRect(cx - 1, ty + 10, 2, 10, col); tft.fillRect(cx - 1, ty + 22, 2, 3, col);
  }
}

void drawDropRow(int filled, uint16_t col) {
  int16_t xs[] = {38, 64, 90};
  for (int i = 0; i < 3; i++) {
    uint16_t c = (i < filled) ? col : COLOR_MUTED;
    tft.fillTriangle(xs[i], 137, xs[i] - 5, 146, xs[i] + 5, 146, c); tft.fillCircle(xs[i], 146, 5, c);
  }
}

void showCritical(int pct) {
  tft.fillScreen(COLOR_BG); drawHeader("AGRO MONITOR", COLOR_RED_DK);
  tft.drawRect(2, 20, 124, 128, COLOR_RED); drawDrop(64, 28, 0, COLOR_RED);
  drawCentered("TANAH KERING!", 80, COLOR_RED); drawCentered("Kadar air sangat rendah", 91, COLOR_RED_DK);
  drawSoilBar(pct, COLOR_RED); drawCentered("TAMBAH AIR!", 138, COLOR_RED);
  drawFooter("STATUS: KRITIS", COLOR_RED_DK);
}

void showWarning(int pct) {
  tft.fillScreen(COLOR_BG); drawHeader("AGRO MONITOR", C_AMBER_DK);
  tft.drawRect(2, 20, 124, 128, C_AMBER); drawDrop(64, 28, 1, C_AMBER);
  drawCentered("AIR MENIPIS", 80, C_AMBER); drawCentered("Kadar air rendah", 91, C_AMBER_DK);
  drawSoilBar(pct, C_AMBER); drawDropRow(1, C_AMBER); drawFooter("STATUS: WARNING", C_AMBER_DK);
}

void showNormal(int pct) {
  tft.fillScreen(COLOR_BG); drawHeader("AGRO MONITOR", COLOR_GREEN_DK);
  tft.drawRect(2, 20, 124, 128, COLOR_GREEN); drawDrop(64, 28, 2, COLOR_GREEN);
  drawCentered("AIR CUKUP", 80, COLOR_GREEN); drawCentered("Tanaman terhidrasi", 91, COLOR_GREEN_DK);
  drawSoilBar(pct, COLOR_GREEN); drawDropRow(3, COLOR_GREEN); drawFooter("STATUS: NORMAL", COLOR_GREEN_DK);
}

// ============================================================
// FUNGSI MENGGAMBAR KERANGKA UI UTAMA DASHBOARD
// ============================================================
void drawLayoutUI() {
  tft.fillScreen(C_BG); tft.setTextSize(1); tft.setTextColor(C_CYAN);
  tft.setCursor(28, 5); tft.print("AGRO MONITOR");

  tft.drawRoundRect(2, 18, 60, 62, 4, C_ORANGE); tft.setCursor(22, 24); tft.setTextColor(C_ORANGE); tft.print("SUHU");
  tft.fillRoundRect(6, 62, 40, 3, 1, C_ORANGE); tft.fillRoundRect(48, 62, 10, 3, 1, 0x3186);  
  
  tft.drawRoundRect(66, 18, 60, 62, 4, C_CYAN); tft.setCursor(86, 24); tft.setTextColor(C_CYAN); tft.print("HUMID");
  tft.fillRoundRect(70, 62, 40, 3, 1, C_CYAN); tft.fillRoundRect(112, 62, 10, 3, 1, 0x3186); 

  tft.drawRoundRect(2, 84, 60, 62, 4, C_GREEN); tft.setCursor(22, 90); tft.setTextColor(C_GREEN); tft.print("TANAH");
  tft.fillRoundRect(6, 128, 30, 3, 1, C_GREEN); tft.fillRoundRect(38, 128, 20, 3, 1, 0x3186);

  tft.drawRoundRect(66, 84, 60, 62, 4, C_YELLOW); tft.setCursor(86, 90); tft.setTextColor(C_YELLOW); tft.print("CAHAYA");
  tft.fillRoundRect(70, 128, 45, 3, 1, C_YELLOW); tft.fillRoundRect(117, 128, 5, 3, 1, 0x3186);

  tft.fillCircle(6, 154, 2, C_GREEN); tft.setCursor(12, 151); tft.setTextColor(C_GREEN); tft.print("SYS:OK");
}

// ============================================================
// FUNGSI BALAS PESAN TELEGRAM
// ============================================================
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (text == "/ceksuhu") {
      float t = dht.readTemperature(); float h = dht.readHumidity();
      int rawSoil = analogRead(SOIL_PIN); int rawLDR = analogRead(LDR_PIN);
      bool errDHT = (isnan(t) || isnan(h)); bool errSoil = (rawSoil <= 5 || rawSoil >= 4090); bool errLDR = (rawLDR <= 5 || rawLDR >= 4090);
      int s = constrain(map(rawSoil, airKering, airBasah, 0, 100), 0, 100);
      int c = map(rawLDR, 0, 4095, 0, 1000);

      String response = "📊 *STATUS TANAMAN SAAT INI:*\n\n";
      response += "🌡️ *Suhu:* " + (errDHT ? String("Data tidak tersedia") : (String(t, 1) + " °C")) + "\n";
      response += "💧 *Kelembaban:* " + (errDHT ? String("Data tidak tersedia") : (String(h, 0) + " %")) + "\n";
      response += "🌱 *Tanah:* " + (errSoil ? String("Data tidak tersedia") : (String(s) + " %")) + "\n";
      response += "☀️ *Cahaya:* " + (errLDR ? String("Data tidak tersedia") : (String(c) + " lux")) + "\n";

      String tsStr = String(millis()/1000) + "s"; 
      if (WiFi.status() != WL_CONNECTED) { showFailedNoWifi(tsStr.c_str()); } 
      else {
        bool sent = bot.sendMessage(chat_id, response, "Markdown");
        if (sent) showSuccess(chat_id.c_str(), tsStr.c_str());
      }
      delay(3000); drawLayoutUI(); 
    }
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_MERAH, OUTPUT); pinMode(LED_BIRU, OUTPUT); pinMode(LDR_PIN, INPUT);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2); // Layar kebalik 180 derajat
  
  tft.fillScreen(C_BG);
  drawCentered("AGRO MONITOR", 70, COLOR_GREEN);
  drawCentered("Connecting WiFi...", 84, COLOR_MUTED);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  client.setInsecure(); 
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Inisialisasi Waktu Internet (WIB = UTC+7 -> 7 * 3600 detik)
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  drawLayoutUI(); 
  bot.sendMessage(CHAT_ID, "🌱 Agro Monitor Ready! Ketik /ceksuhu.\nJadwal laporan: 07:00 & 17:00.", "");
}

// ============================================================
// MAIN LOOP
// ============================================================
void loop() {
  // 1. Cek Pesan Masuk
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  if (numNewMessages) handleNewMessages(numNewMessages);

  // 2. Baca Sensor
  float t = dht.readTemperature(); float h = dht.readHumidity();
  int rawSoil = analogRead(SOIL_PIN); int rawLDR = analogRead(LDR_PIN);

  bool errDHT = (isnan(t) || isnan(h));
  bool errSoil = (rawSoil <= 5 || rawSoil >= 4090);
  bool errLDR = (rawLDR <= 5 || rawLDR >= 4090);

  int s = errSoil ? 0 : constrain(map(rawSoil, airKering, airBasah, 0, 100), 0, 100);
  int c = errLDR ? 0 : map(rawLDR, 0, 4095, 0, 1000); 

  // --- LOGIKA JADWAL LAPORAN (07:00 dan 17:00) ---
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    // Format Jam:Menit untuk Footer TFT
    char timeStringBuff[10];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
    tft.fillRect(95, 151, 30, 10, C_BG);
    tft.setCursor(95, 151); tft.setTextColor(C_TEXT); tft.print(timeStringBuff);

    int jam = timeinfo.tm_hour;
    int menit = timeinfo.tm_min;

    // Laporan Pagi Jam 07:00
    if (jam == 7 && menit == 0 && !reportPagiTerkirim) {
      String rep = "🌅 *LAPORAN PAGI*\nSuhu: " + String(t, 1) + "°C\nTanah: " + String(s) + "%\nSemangat hari ini!";
      bot.sendMessage(CHAT_ID, rep, "Markdown");
      reportPagiTerkirim = true;
    } else if (jam != 7) {
      reportPagiTerkirim = false; // Reset status kalau udah lewat jam 7
    }

    // Laporan Sore Jam 17:00
    if (jam == 17 && menit == 0 && !reportSoreTerkirim) {
      String rep = "🌇 *LAPORAN SORE*\nSuhu: " + String(t, 1) + "°C\nTanah: " + String(s) + "%\nWaktunya istirahat!";
      bot.sendMessage(CHAT_ID, rep, "Markdown");
      reportSoreTerkirim = true;
    } else if (jam != 17) {
      reportSoreTerkirim = false;
    }
  }

  // --- LOGIKA POP-UP WARNING & NOTIFIKASI TELEGRAM OTOMATIS (5 MENIT) ---
  static unsigned long lastWarningMillis = 0;
  static int lastAlertState = 0; 
  
  // Status alert: 2 = Kritis, 1 = Warning, 0 = Aman
  int currentAlertState = (s < 20 || c < 100) ? 2 : ((s < 40 || c < 200) ? 1 : 0);

  if (currentAlertState != 0) { 
    // Trigger jika status memburuk ATAU sudah 5 menit (300.000 ms)
    if (currentAlertState != lastAlertState || (millis() - lastWarningMillis >= 300000)) { 
      
      // Kirim Notifikasi Telegram karena Air/Cahaya Kurang
      String alertMsg = "🚨 *PERINGATAN TANAMAN!*\n";
      if (s < 40) alertMsg += "💧 Air kurang! Tanah kering (" + String(s) + "%).\n";
      if (c < 200) alertMsg += "☀️ Cahaya kurang! Terlalu gelap (" + String(c) + " lux).\n";
      alertMsg += "Segera cek lokasi.";
      bot.sendMessage(CHAT_ID, alertMsg, "Markdown");

      // Tampilkan UI Pop-up di Layar TFT
      if (currentAlertState == 2) showCritical(s);
      else showWarning(s);
      
      delay(4000);     
      drawLayoutUI();  
      
      lastWarningMillis = millis(); 
      lastAlertState = currentAlertState;
    }
  } else {
    if (lastAlertState != 0) {
       showNormal(s); delay(4000); drawLayoutUI();
    }
    lastAlertState = 0;
  }

  // --- UPDATE ANGKA DI DASHBOARD UTAMA ---
  String statusSuhu = errDHT ? "-" : ((t > 35) ? "Panas" : "Normal");
  String statusHumid = errDHT ? "-" : ((h > 70) ? "Tinggi" : "Normal");
  String statusTanah = errSoil ? "-" : ((s < 40) ? "Kering" : "Lembab");
  String statusCahaya = errLDR ? "-" : ((c > 600) ? "Cerah" : "Redup");

  tft.fillRect(6, 38, 52, 16, C_BG); 
  tft.setCursor(6, 40); tft.setTextColor(C_ORANGE); tft.setTextSize(2);
  if(errDHT) { tft.print("-"); } else { tft.print(t, 1); tft.setTextSize(1); tft.print("c"); }
  tft.fillRect(6, 68, 52, 10, C_BG); tft.setCursor(6, 70); tft.setTextColor(C_ORANGE); tft.setTextSize(1); tft.print(statusSuhu);

  tft.fillRect(70, 38, 52, 16, C_BG);
  tft.setCursor(70, 40); tft.setTextColor(C_CYAN); tft.setTextSize(2);
  if(errDHT) { tft.print("-"); } else { tft.print(h, 0); tft.setTextSize(1); tft.print("%"); }
  tft.fillRect(70, 68, 52, 10, C_BG); tft.setCursor(70, 70); tft.setTextColor(C_CYAN); tft.setTextSize(1); tft.print(statusHumid);

  tft.fillRect(6, 104, 52, 16, C_BG);
  tft.setCursor(6, 106); tft.setTextColor(COLOR_GREEN); tft.setTextSize(2);
  if(errSoil) { tft.print("-"); } else { tft.print(s); tft.setTextSize(1); tft.print("%"); }
  tft.fillRect(6, 134, 52, 10, C_BG); tft.setCursor(6, 136); tft.setTextColor(COLOR_GREEN); tft.setTextSize(1); tft.print(statusTanah);

  tft.fillRect(70, 104, 52, 16, C_BG);
  tft.setCursor(70, 106); tft.setTextColor(C_YELLOW); tft.setTextSize(2);
  if(errLDR) { tft.print("-"); } else { tft.print(c); }
  tft.fillRect(70, 134, 52, 10, C_BG); tft.setCursor(70, 136); tft.setTextColor(C_YELLOW); tft.setTextSize(1); tft.print(statusCahaya);

  // LOGIKA LED FISIK
  if (errDHT || errSoil || s < 40 || t > 35) {
    digitalWrite(LED_MERAH, HIGH); digitalWrite(LED_BIRU, LOW);
  } else {
    digitalWrite(LED_MERAH, LOW); digitalWrite(LED_BIRU, HIGH);
  }

  delay(2000); 
}
