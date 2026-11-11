#ifndef FETCH_H
#define FETCH_H
#include <Arduino.h>

void fetchUsage();
time_t parseISO(const String &iso);
String remainStr(const String &resetISO);
String extractTime(const String &iso);

#endif
