#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

struct Settings {
  char ssid[32];
  char password[32];
};

extern Settings settings;

void readSettings();
void saveSettings();

#endif
