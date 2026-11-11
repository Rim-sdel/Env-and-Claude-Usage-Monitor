#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "MHZ19.h"
#include "WiFiGeneric.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "fetch.h"
#include "globals.h"
#include "printclaude.h"
#include "secrets.h"
#include "sensor.h"
#include "splash.h"

// -- WiFi --
HTTPClient http;

// -- Claude state --
int fiveHourPct = -1;
int sevenDayPct = -1;
String fiveHourReset = "";
String sevenDayReset = "";
unsigned long lastFetch = 0;
const unsigned long FETCH_INTERVAL = 30000; // 30sec

// -- LCD monior --
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
#define I2C_SDA 6
#define I2C_SCL 7

// --fonts for env --
uiConfig ui = {
  u8g2_font_5x7_tf,     // 見出し
  u8g2_font_4x6_tf,     // 小さい見出し
  u8g2_font_crox4h_tf,  // 数字
  u8g2_font_crox2hb_tn, // 小さい数字

  64,
  128,
  2,
  5,
};

// --- MH-Z19 センサーの設定 ---
MHZ19 myMHZ19;              // Constructor for library
HardwareSerial mySerial(1); // (Uno example) create device to MH-Z19 serial
#define RX_PIN 5            // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 10           // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600       // Device to MH-Z19 Serial baudrate (should not be changed)

unsigned long getDataTimer = 0;

// -- BME280 --
Adafruit_BME280 bme; // I2C

// -- button --
const int buttonPin = 3;
bool lastButtonState = HIGH;
bool stableState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// -- Mode --
bool isEnvMonitor = true;
bool isDrawEnvMonitor = true;

void setup() {
  Serial.begin(9600); // Device to serial monitor feedback
  while (!Serial)
    ;

  Serial.println("== Booting... ==");
  pinMode(buttonPin, INPUT_PULLUP);
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("Wire OK");

  // -- monitor --
  u8g2.begin();
  // スプラッシュ表示
  drawSplash();

  // WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); // 安価なボード用の処理
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. -- IP: ");
  Serial.println(WiFi.localIP());

  // NTP時刻同期（カウントダウン計算用）
  configTime(0, 0, "pool.ntp.org");
  Serial.println("NTP sync...");
  struct tm t;
  while (!getLocalTime(&t)) {
    delay(200);
  }
  Serial.println("NTP OK");

  // -- BME280 --
  unsigned status = bme.begin(0x76);
  if (!status) {
    Serial.println("BME280 not found");
    while (1)
      ;
  }

  // センサー通信開始
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN); // (Uno example) device to MH-Z19 serial start
  Serial.println("mySerial OK");

  myMHZ19.begin(mySerial);   // *Serial(Stream) refence must be passed to library begin().
  myMHZ19.autoCalibration(); // Turn auto calibration ON (OFF autoCalibration(false))

  Serial.println("== Setup copleted! ==");
}

void loop() {
  bool read = digitalRead(buttonPin);

  if (read != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (stableState == HIGH && read == LOW) {
      Serial.println("ぽちっ");
      isEnvMonitor = !isEnvMonitor;
      Serial.printf("今の画面： %s\n", isEnvMonitor ? "環境モニター" : "Claudeモニター");
    }
    stableState = read;
  }
  lastButtonState = read;

  if (isEnvMonitor) {

    if (!isDrawEnvMonitor) {
      envMode();
      isDrawEnvMonitor = true;
    }

    if (millis() - getDataTimer >= 5000) {
      getDataTimer = millis();
      envMode();
    }
  } else {
    if (isDrawEnvMonitor) {
      fetchUsage();
      drawClaude();
      isDrawEnvMonitor = false;
    }
    if (millis() - lastFetch >= FETCH_INTERVAL) {
      lastFetch = millis();
      fetchUsage();
      drawClaude();
    }
  }
}
