#include "splash.h"
#include "globals.h"
#include <Arduino.h>
#include <U8g2lib.h>
// 突然変異カニ 16x16（スプラッシュ用）
static const unsigned char PROGMEM mutant_16x16[] = {
  0x00,
  0x00, // row 0
  0x00,
  0x00, // row 1
  0x00,
  0x00, // row 2
  0x08,
  0x10, // row 3  (触角先端)
  0x10,
  0x08, // row 4  (触角)
  0x10,
  0x08, // row 5
  0xE6,
  0x67, // row 6  (ハサミ+体)
  0xFC,
  0x3F, // row 7
  0xA2,
  0x45, // row 8  (ハサミ+体+目)
  0xE0,
  0x07, // row 9  (体)
  0xE0,
  0x07, // row 10
  0xA0,
  0x05, // row 11 (脚)
  0xA0,
  0x05, // row 12
  0x00,
  0x00, // row 13
  0x00,
  0x00, // row 14
  0x00,
  0x00, // row 15
};

// =============================================
//  スプラッシュスクリーン（突然変異体を2倍で描画）
// =============================================

void drawXBM_2x(int ox, int oy, int w, int h, const unsigned char *bmp) {
  int bytesPerRow = (w + 7) / 8;
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      int byteIdx = row * bytesPerRow + (col / 8);
      uint8_t b = pgm_read_byte(&bmp[byteIdx]);
      if (b & (1 << (col % 8))) {
        // 1ピクセルを2x2で描画
        u8g2.drawBox(ox + col * 2, oy + row * 2, 2, 2);
      }
    }
  }
}

void drawSplash() {
  u8g2.clearBuffer();
  // 16x16 を 2倍 → 32x32、画面中央に配置
  drawXBM_2x((128 - 32) / 2, (64 - 32) / 2 - 4, 16, 16, mutant_16x16);

  u8g2.setFont(u8g2_font_5x8_tr);
  const char *msg = "booting...";
  int tw = u8g2.getStrWidth(msg);
  u8g2.drawStr((128 - tw) / 2, 55, msg);
  u8g2.sendBuffer();
  delay(2000);
}
