#ifndef GLOBALS_H
#define GLOBALS_H

#include "Adafruit_BME280.h"
#include "HTTPClient.h"
#include "MHZ19.h"
#include <U8g2lib.h>

extern HTTPClient http;
// モニター関連
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

struct uiConfig {
  const uint8_t *HEAD;
  const uint8_t *HEAD_S;
  const uint8_t *NUM;
  const uint8_t *NUM_S;
  int monitor_h;
  int monitor_w;
  int margin_h;
  int margin_w;
};
extern uiConfig ui;

// sensor
extern MHZ19 myMHZ19;
extern Adafruit_BME280 bme;

extern int fiveHourPct;
extern int sevenDayPct;
extern String fiveHourReset;
extern String sevenDayReset;

#endif
