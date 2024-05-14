#include "settings.h"
#include <EEPROM.h>

Settings settings;

void readSettings() {
  EEPROM.begin(512);
  EEPROM.get(0, settings);
  if (settings.ssid[0] == 0xFF) {
    memset(&settings, 0, sizeof(Settings));
  }
  EEPROM.end();
}

void saveSettings() {
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.commit();
  EEPROM.end();
}
