#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include "time.h"
#include "DHT.h"

// Konfigurasi OLED SH1106
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WiFi
const char* ssid     = ""; // SSID WiFi
const char* password = ""; // Password WiFi

// NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;  // GMT+7 WIB
const int   daylightOffset_sec = 0;

#define DHTPIN 4      // Pin DATA DHT22
#define DHTTYPE DHT22 // Sensor DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  // Inisialisasi OLED
  if(!display.begin(0x3C, true)) {  // alamat I2C biasanya 0x3C
    Serial.println(F("SH1106 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0,0);
  display.println("Connecting WiFi...");
  display.display();

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected!");
  display.display();

  // Sinkronisasi waktu dari NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  dht.begin();
}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Time Error");
    display.display();
    return;
  }

  display.clearDisplay();
  
  // Tampilkan jam
  display.setTextSize(2);
  display.setCursor(19,5);
  display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  // Tampilkan tanggal
  display.setTextSize(1);
  display.setCursor(36,30);
  display.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon+1, 1900+timeinfo.tm_year);

  // Tampilkan DHT22 (suhu & kelembapan)
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  // Tampilkan DHT22 (suhu & kelembapan) bertumpuk sederhana
  display.setTextSize(1);

  if (isnan(humidity) || isnan(temperature)) {
      display.setCursor(35, 45); // kira-kira tengah horizontal
      display.println("DHT Error");
  } else {
      display.setCursor(12, 45); // Temp di atas
      display.printf("Temprature: %.1f C", temperature);

      display.setCursor(18, 55); // Hum di bawah
      display.printf("Humidity : %.1f%%", humidity);
  }

  display.display();
  delay(1000);
}

