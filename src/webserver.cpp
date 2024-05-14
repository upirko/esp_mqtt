#include "webserver.h"
#include "settings.h"
#include <base64.h>

ESP8266WebServer server(80);

const char* www_username = "admin";
const char* www_password = "admin";

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}

void handleRoot() {
  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();

  String form = "<form method='post' action='/save'>"
              "SSID:<br><input type='text' name='ssid' value='" + String(settings.ssid) + "'><br>"
              "Password:<br><input type='password' name='password' value='" + String(settings.password) + "'><br>"
              "Device name:<br><input type='text' name='device_name' value='" + String(settings.device_name) + "'><br>"
              "MQTT Server:<br><input type='text' name='mqtt_server' value='" + String(settings.mqtt_server) + "'><br>"
              "MQTT User:<br><input type='text' name='mqtt_user' value='" + String(settings.mqtt_user) + "'><br>"
              "MQTT Password:<br><input type='password' name='mqtt_password' value='" + String(settings.mqtt_password) + "'><br><br>"
              "<input type='submit' value='Save'></form>";

  server.send(200, "text/html", form);
}

void handleSave() {
  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();

  server.arg("ssid").toCharArray(settings.ssid, sizeof(settings.ssid));
  server.arg("password").toCharArray(settings.password, sizeof(settings.password));
  server.arg("mqtt_server").toCharArray(settings.mqtt_server, sizeof(settings.mqtt_server));
  server.arg("mqtt_user").toCharArray(settings.mqtt_user, sizeof(settings.mqtt_user));
  server.arg("mqtt_password").toCharArray(settings.mqtt_password, sizeof(settings.mqtt_password));
  server.arg("device_name").toCharArray(settings.device_name, sizeof(settings.device_name));

  saveSettings();

  server.send(200, "text/html", "Settings saved. Device will reboot.");
  delay(2000);
  ESP.restart();
}
