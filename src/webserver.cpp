#include "webserver.h"
#include "settings.h"
#include <LittleFS.h>
#include <base64.h>

ESP8266WebServer server(80);

const char* www_username = "admin";
const char* www_password = "admin";

void setupWebServer() {
  server.on("/settings", HTTP_GET, handleRoot);
  server.on("/settings/save", HTTP_POST, handleSave);
  LittleFS.begin();
  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void handleRoot() {
  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();

  String form = "<form method='post' action='/settings/save'>"
              "SSID:<br><input type='text' name='ssid' value='" + String(settings.ssid) + "'><br>"
              "Password:<br><input type='password' name='password' value='" + String(settings.password) + "'><br>"
              "<input type='submit' value='Save'></form>";

  server.send(200, "text/html", form);
}

void handleSave() {
  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();

  server.arg("ssid").toCharArray(settings.ssid, sizeof(settings.ssid));
  server.arg("password").toCharArray(settings.password, sizeof(settings.password));
  
  saveSettings();

  server.send(200, "text/html", "Settings saved. Device will reboot.");
  delay(2000);
  ESP.restart();
}
