#include "MHZ19.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <random>

#include "globals.h"
#include "print.h"

int head_s_height = 48;
int under = ui.monitor_h - ui.margin_h;

void getEnv(int &CO2, int &TEMP, int &HUM, int &BARO) {
  Serial.println("--------------------");

  CO2 = myMHZ19.getCO2();
  Serial.print("CO2: ");
  Serial.print(CO2);
  Serial.println(" ppm");

  TEMP = bme.readTemperature();
  Serial.print("Temp: ");
  Serial.print(TEMP);
  Serial.println(" °C");

  HUM = bme.readHumidity();
  Serial.print("Hum: ");
  Serial.print(HUM);
  Serial.println(" %");

  BARO = bme.readPressure() / 100.0F;
  Serial.print("Baro: ");
  Serial.print(BARO);
  Serial.println(" hPa");
}

void updateMonitor(int CO2, int TEMP, int HUM, int BARO) {

  u8g2.clearBuffer(); // 内部バッファをクリア

  printSet(ui.HEAD, 57, 5 + ui.margin_h, "CO2");
  printSet(ui.NUM, 48, 30, CO2);
  printSet(ui.HEAD, 80, 30, "  ppm");

  u8g2.drawHLine(ui.margin_w, 38, ui.monitor_w - ui.margin_w * 2);

  printSet(ui.HEAD_S, 5, head_s_height, "TEMP");
  printSet(ui.NUM_S, 15, under, TEMP);
  printSet(ui.HEAD_S, 33, under, "\xb0"
                                 "C");

  printSet(ui.HEAD_S, 45, head_s_height, "HUM");
  printSet(ui.NUM_S, 55, under, HUM);
  printSet(ui.HEAD_S, 73, under, "%");

  printSet(ui.HEAD_S, 85, head_s_height, "BARO");
  printSet(ui.NUM_S, 91, under, BARO);

  u8g2.sendBuffer(); // バッファの内容をディスプレイに転送
}

void envMode() {
  int CO2, TEMP, HUM, BARO;
  getEnv(CO2, TEMP, HUM, BARO);
  updateMonitor(CO2, TEMP, HUM, BARO);
}
