#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <DHT.h>

// --- PIN HARDWARE ---
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2
#define DHTPIN     15
#define DHTTYPE    DHT22 
#define SOIL_PIN   34
#define LDR_PIN    35   // <-- Pin AO dari Sensor Cahaya colok ke sini
#define LED_MERAH  12
#define LED_BIRU   14

// --- WARNA DASHBOARD UTAMA ---
#define C_BG       0x0821  
#define C_ORANGE   0xFC00  
#define C_CYAN     0x04FF  
#define C_GREEN    0x07E0  
#define C_YELLOW   0xFFE0  
#define C_TEXT     0x7BEF  

// --- WARNA UI WARNING DROP ---
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

// Kalibrasi Tanah
const int airKering = 3200; 
const int airBasah  = 1500;

// ============================================================
// FUNGSI UI HELPER
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

void drawCentered(const char* text, int16_t y, uint16_t color, uint8_t size = 1) {
  tft.setTextSize(size);
  int16_t x = (128 - strlen(text) * 6 * size) / 2;
  tft.setTextColor(color); tft.setCursor(x, y); tft.print(text);
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
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_MERAH, OUTPUT); 
  pinMode(LED_BIRU, OUTPUT); 
  pinMode(LDR_PIN, INPUT);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2); // Layar diputar 180 derajat
  
  // Langsung render Dashboard tanpa nunggu WiFi
  drawLayoutUI(); 
}

// ============================================================
// MAIN LOOP
// ============================================================
void loop() {
  // 1. Baca Sensor
  float t = dht.readTemperature(); 
  float h = dht.readHumidity();
  int rawSoil = analogRead(SOIL_PIN); 
  int rawLDR = analogRead(LDR_PIN);

  // Deteksi Sensor Copot/Rusak
  bool errDHT = (isnan(t) || isnan(h));
  bool errSoil = (rawSoil <= 5 || rawSoil >= 4090);
  bool errLDR = (rawLDR <= 5 || rawLDR >= 4090);

  // Konversi Data (Batas 0-100% dan 0-1000 lux)
  int s = errSoil ? 0 : constrain(map(rawSoil, airKering, airBasah, 0, 100), 0, 100);
  int c = errLDR ? 0 : map(rawLDR, 0, 4095, 0, 1000); 

  // --- LOGIKA POP-UP WARNING (MUNcul 1x TIAP 5 MENIT JIKA BAHAYA) ---
  static unsigned long lastWarningMillis = 0;
  static int lastAlertState = 0; 
  
  // Status alert: 2 = Kritis, 1 = Warning, 0 = Aman
  int currentAlertState = (s < 20 || c < 100) ? 2 : ((s < 40 || c < 200) ? 1 : 0);

  if (currentAlertState != 0) { 
    // Trigger pop-up jika status memburuk ATAU sudah 5 menit (300.000 ms)
    if (currentAlertState != lastAlertState || (millis() - lastWarningMillis >= 300000)) { 
      
      if (currentAlertState == 2) showCritical(s);
      else showWarning(s);
      
      delay(4000);     // Tahan Pop-up 4 detik
      drawLayoutUI();  // Hapus Pop-up & balikin ke Dashboard
      
      lastWarningMillis = millis(); 
      lastAlertState = currentAlertState;
    }
  } else {
    // Balik Normal
    if (lastAlertState != 0) {
       showNormal(s); 
       delay(4000); 
       drawLayoutUI();
    }
    lastAlertState = 0;
  }

  // --- UPDATE ANGKA DI DASHBOARD UTAMA ---
  String statusSuhu = errDHT ? "-" : ((t > 35) ? "Panas" : "Normal");
  String statusHumid = errDHT ? "-" : ((h > 70) ? "Tinggi" : "Normal");
  String statusTanah = errSoil ? "-" : ((s < 40) ? "Kering" : "Lembab");
  String statusCahaya = errLDR ? "-" : ((c > 600) ? "Cerah" : "Redup");

  // Update SUHU
  tft.fillRect(6, 38, 52, 16, C_BG); 
  tft.setCursor(6, 40); tft.setTextColor(C_ORANGE); tft.setTextSize(2);
  if(errDHT) { tft.print("-"); } else { tft.print(t, 1); tft.setTextSize(1); tft.print("c"); }
  tft.fillRect(6, 68, 52, 10, C_BG); 
  tft.setCursor(6, 70); tft.setTextColor(C_ORANGE); tft.setTextSize(1); tft.print(statusSuhu);

  // Update HUMID
  tft.fillRect(70, 38, 52, 16, C_BG);
  tft.setCursor(70, 40); tft.setTextColor(C_CYAN); tft.setTextSize(2);
  if(errDHT) { tft.print("-"); } else { tft.print(h, 0); tft.setTextSize(1); tft.print("%"); }
  tft.fillRect(70, 68, 52, 10, C_BG); 
  tft.setCursor(70, 70); tft.setTextColor(C_CYAN); tft.setTextSize(1); tft.print(statusHumid);

  // Update TANAH
  tft.fillRect(6, 104, 52, 16, C_BG);
  tft.setCursor(6, 106); tft.setTextColor(COLOR_GREEN); tft.setTextSize(2);
  if(errSoil) { tft.print("-"); } else { tft.print(s); tft.setTextSize(1); tft.print("%"); }
  tft.fillRect(6, 134, 52, 10, C_BG); 
  tft.setCursor(6, 136); tft.setTextColor(COLOR_GREEN); tft.setTextSize(1); tft.print(statusTanah);

  // Update CAHAYA
  tft.fillRect(70, 104, 52, 16, C_BG);
  tft.setCursor(70, 106); tft.setTextColor(C_YELLOW); tft.setTextSize(2);
  if(errLDR) { tft.print("-"); } else { tft.print(c); }
  tft.fillRect(70, 134, 52, 10, C_BG); 
  tft.setCursor(70, 136); tft.setTextColor(C_YELLOW); tft.setTextSize(1); tft.print(statusCahaya);

  // Waktu Footer (Uptime System Format Menit:Detik)
  int upTimeSec = millis() / 1000;
  char timeStr[10];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", (upTimeSec / 60) % 60, upTimeSec % 60);
  tft.fillRect(95, 151, 30, 10, C_BG);
  tft.setCursor(95, 151); tft.setTextColor(C_TEXT); tft.print(timeStr);

  // --- LOGIKA LED FISIK ---
  if (errDHT || errSoil || errLDR || s < 40 || c < 100 || t > 35) {
    digitalWrite(LED_MERAH, HIGH); 
    digitalWrite(LED_BIRU, LOW);
  } else {
    digitalWrite(LED_MERAH, LOW); 
    digitalWrite(LED_BIRU, HIGH);
  }

  delay(2000); // Jeda pembacaan biar layar nggak kedip dan sensor rileks
}
