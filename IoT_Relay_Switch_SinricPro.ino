#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

#define WIFI_SSID     "Poco m4"
#define WIFI_PASS     "12345678"
#define APP_KEY       "e6cf5116-269d-42c0-994c-9d3c1558ae8f"
#define APP_SECRET    "80d71f97-5b10-4cfd-bdd8-23ae8f753576-c25b11e0-f25d-45f8-be2a-98d677c89b46"
#define BAUD_RATE     9600
#define wifiLed       LED_BUILTIN  // Optional status LED

#define RELAY_PIN     D3  // GPIO0
#define DEVICE_ID     "68551284030990a558b8b654"

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("[SinricPro] Device %s: %s\r\n", deviceId.c_str(), state ? "ON" : "OFF");
  digitalWrite(RELAY_PIN, !state);  // Relay is active LOW
  return true;
}

void setupWiFi() {
  Serial.print("[WiFi] Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }
  Serial.printf("\n[WiFi] Connected: %s\n", WiFi.localIP().toString().c_str());
  digitalWrite(wifiLed, LOW);
}

void setupSinricPro() {
  SinricProSwitch &mySwitch = SinricPro[DEVICE_ID];
  mySwitch.onPowerState(onPowerState);

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);  // OFF initially

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Relay OFF (active LOW)

  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();  // Listen for commands from SinricPro
}