#include "globals.h"
#include <U8g2lib.h>

void printSet(const uint8_t *FONT, int x, int y, const char *TEXT) {
  u8g2.setFont(FONT);
  u8g2.setCursor(x, y);
  u8g2.print(TEXT);
}

void printSet(const uint8_t *FONT, int x, int y, int NUM) {
  u8g2.setFont(FONT);
  u8g2.setCursor(x, y);
  u8g2.print(NUM);
}
