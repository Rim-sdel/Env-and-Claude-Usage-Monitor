#include "globals.h"
#include <fetch.h>
#include <stdint.h>

// ---- レイアウト定数 ----
static const int barL = 10;               // バー左端
static const int barW = 128 - (barL * 2); // バー幅（全幅に近い）
static const int barRight = barL + barW;  // 125

// 5時間セクション
static const int sec1_iconY = 25; // アイコンのy
static const int sec1_numY = 33;  // 数字のベースライン
static const int sec1_barY = 34;  // バーのy
static const int sec1_barH = 4;

// 7日セクション
static const int sec2_iconY = 47;
static const int sec2_numY = 54;
static const int sec2_barY = 56;
static const int sec2_barH = 4;

// フォント
const uint8_t *cNUM = u8g2_font_profont17_tn;
const uint8_t *cNUMs = u8g2_font_helvR08_tf;
const uint8_t *cMETA = u8g2_font_4x6_tf;

// ---- ビットマップデータ ----
// clang-format off
static const unsigned char PROGMEM crab_24x24[] = {
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0xF0, 0xFF, 0x0F,
    0xF0, 0xFF, 0x0F,
    0xF0, 0xFC, 0x09,
    0xF0, 0xFC, 0x09,
    0xFE, 0xFF, 0x7F,
    0xFE, 0xFF, 0x7F,
    0xFE, 0xFF, 0x7F,
    0xF0, 0xFF, 0x0F,
    0xF0, 0xFF, 0x0F,
    0xF0, 0xFF, 0x0F,
    0xF0, 0xFF, 0x0F,
    0xF0, 0xFF, 0x0F,
    0x30, 0xC3, 0x0C,
    0x30, 0xC3, 0x0C,
    0x30, 0xC3, 0x0C,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
};
// clang-format on

static const unsigned char PROGMEM icon_clock[] = {
  0x1C, 0x2A, 0x49, 0x59, 0x41, 0x22, 0x1C,
};

static const unsigned char PROGMEM icon_cal[] = {
  0x22, 0x22, 0x7F, 0x41, 0x59, 0x55, 0x7F,
};

// ---- 数字＋%（右揃え） ----
void drawPctRight(int rightX, int y, int pct, const uint8_t *mainFont, const uint8_t *smallFont) {
  char numBuf[6];
  snprintf(numBuf, sizeof(numBuf), "%d", pct);

  // %の幅を測る
  u8g2.setFont(smallFont);
  int pctW = u8g2.getStrWidth("%");

  // 数字の幅を測る
  u8g2.setFont(mainFont);
  int numW = u8g2.getStrWidth(numBuf);

  // 右端から逆算
  int totalW = numW + 1 + pctW;
  int startX = rightX - totalW;

  u8g2.setFont(mainFont);
  u8g2.drawStr(startX, y, numBuf);
  u8g2.setFont(smallFont);
  u8g2.drawStr(startX + numW + 1, y, "%");
}

// ---- プログレスバー描画 ----
void drawBar(int x, int y, int w, int h, int pct) {
  u8g2.drawFrame(x, y, w, h);
  if (pct > 0) {
    int fillW = ((long)pct * (w - 2)) / 100;
    if (fillW < 1)
      fillW = 1;
    u8g2.drawBox(x + 1, y + 1, fillW, h - 2);
  }
}

// ---- メイン描画 ----
void drawClaude() {
  u8g2.clearBuffer();

  // ---- カニ 24x24（中央上部） ----
  u8g2.drawXBM(52, 1, 24, 24, crab_24x24);

  // ======== 5時間セッション ========
  // アイコン（左端）
  u8g2.drawXBM(barL, sec1_iconY, 7, 7, icon_clock);

  if (fiveHourPct >= 0) {
    // リセット時刻（アイコンの右隣）
    String r = remainStr(fiveHourReset);
    if (r.length() > 0) {
      u8g2.setFont(cMETA);
      u8g2.drawStr(barL + 9, sec1_iconY + 6, r.c_str());
    }

    // 数字＋%（右揃え）
    drawPctRight(barRight, sec1_numY, fiveHourPct, cNUM, cNUMs);

    // バー（全幅）
    drawBar(barL, sec1_barY, barW, sec1_barH, fiveHourPct);
  } else {
    u8g2.setFont(cNUMs);
    u8g2.drawStr(barRight - u8g2.getStrWidth("N/A"), sec1_numY, "N/A");
    drawBar(barL, sec1_barY, barW, sec1_barH, 0);
  }

  // ======== 7日制限 ========
  u8g2.drawXBM(barL, sec2_iconY, 7, 7, icon_cal);

  if (sevenDayPct >= 0) {
    String r = remainStr(sevenDayReset);
    if (r.length() > 0) {
      u8g2.setFont(cMETA);
      u8g2.drawStr(barL + 9, sec2_iconY + 6, r.c_str());
    }

    drawPctRight(barRight, sec2_numY, sevenDayPct, cNUM, cNUMs);
    drawBar(barL, sec2_barY, barW, sec2_barH, sevenDayPct);
  } else {
    u8g2.setFont(cNUMs);
    u8g2.drawStr(barRight - u8g2.getStrWidth("N/A"), sec2_numY, "N/A");
    drawBar(barL, sec2_barY, barW, sec2_barH, 0);
  }

  u8g2.sendBuffer();
}
