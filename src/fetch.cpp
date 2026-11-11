#include "globals.h"
#include "secrets.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

void fetchUsage() {
  http.begin(SERVER_URL);
  http.setReuse(true);
  http.setTimeout(10000);

  int code = http.GET();
  if (code == 200) {
    String body = http.getString();
    JsonDocument doc;
    if (!deserializeJson(doc, body)) {
      if (!doc["five_hour"].isNull()) {
        float u = doc["five_hour"]["utilization"] | 0.0f;
        fiveHourPct = min((int)lroundf(u), 100);
        fiveHourReset = doc["five_hour"]["resets_at"].as<String>();
      }
      if (!doc["seven_day"].isNull()) {
        float u = doc["seven_day"]["utilization"] | 0.0f;
        sevenDayPct = min((int)lroundf(u), 100);
        sevenDayReset = doc["seven_day"]["resets_at"].as<String>();
      }
      Serial.printf("[OK] 5h=%d%% 7d=%d%%\n", fiveHourPct, sevenDayPct);
    } else {
      Serial.println("[ERR] JSON parse failed");
    }
  } else {
    Serial.printf("[ERR] HTTP %d\n", code);
    if (code == 401) {
      Serial.println("Cookie expired");
    }
  }
  http.end();
}

// =============================================
//  リセットまでの残り時間を計算して文字列化
// =============================================

// ISO8601 → epoch秒（簡易パーサ、UTCのみ対応）
time_t parseISO(const String &iso) {
  // "2026-11-11T16:20:00..."
  struct tm t = {};
  t.tm_year = iso.substring(0, 4).toInt() - 1900;
  t.tm_mon = iso.substring(5, 7).toInt() - 1;
  t.tm_mday = iso.substring(8, 10).toInt();
  t.tm_hour = iso.substring(11, 13).toInt();
  t.tm_min = iso.substring(14, 16).toInt();
  t.tm_sec = iso.substring(17, 19).toInt();
  return mktime(&t); // UTC前提で gmtime 逆変換
}

// 残り時間を "in 2h13m" / "in 4d 6h" 形式で返す
String remainStr(const String &resetISO) {
  if (resetISO.length() == 0)
    return "";
  time_t now;
  time(&now);
  time_t rst = parseISO(resetISO);
  long diff = (long)difftime(rst, now);
  if (diff <= 0)
    return "now";
  long m = diff / 60;
  long h = m / 60;
  long d = h / 24;
  if (d > 0) {
    char buf[16];
    snprintf(buf, sizeof(buf), "in %ldd %ldh", d, h % 24);
    return String(buf);
  } else {
    char buf[16];
    snprintf(buf, sizeof(buf), "in %ldh%02ldm", h, m % 60);
    return String(buf);
  }
}

// ---------------------------------------------------

String extractTime(const String &iso) {
  // "2026-11-11T16:20:00.855863+00:00" → "16:20"
  int tPos = iso.indexOf('T');
  if (tPos < 0 || iso.length() < (unsigned)(tPos + 6))
    return "??:??";
  return iso.substring(tPos + 1, tPos + 6);
}
