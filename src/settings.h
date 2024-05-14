#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

struct Settings {
  char ssid[32];
  char password[32];
  char mqtt_server[32];
  char mqtt_user[32];
  char mqtt_password[32];
  char device_name[32];
};

extern Settings settings;

void readSettings();
void saveSettings();

#endif
