#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi credentials
const char* ssid = "Poco m4";
const char* password = "12345678";

// ThingSpeak info
const char* ts_write_api_key = "UXV7VT0EA6TIQW6T"; // Your Write API Key
const char* alert_api_key = "PUT-YOUR-ALERT-API-KEY-HERE"; // From ThingSpeak
const char* ts_url = "http://api.thingspeak.com/update";
const char* alert_url = "https://api.thingspeak.com/alerts/send";

// Thresholds
float temp_threshold = 35.0;
float humidity_threshold = 80.0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    delay(20000);
    return;
  }

  Serial.printf("Temp: %.2f °C, Humidity: %.2f %%\n", temperature, humidity);

  WiFiClient client;
  HTTPClient http;

  // Send to ThingSpeak
  http.begin(client, ts_url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "api_key=" + String(ts_write_api_key) +
                    "&field1=" + String(temperature) +
                    "&field2=" + String(humidity);

  int httpCode = http.POST(postData);
  Serial.printf("ThingSpeak HTTP POST: %d\n", httpCode);
  http.end();

  // Check for alert condition
  if (temperature > temp_threshold || humidity > humidity_threshold) {
    Serial.println("Threshold exceeded, sending alert...");

    String alert_subject = "ESP8266 Alert: High Temp or Humidity!";
    String alert_body = "Temp: " + String(temperature) + " °C, Humidity: " + String(humidity) + " %";

    http.begin(client, alert_url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("ThingSpeak-Alerts-API-Key", alert_api_key);

    String jsonPayload = "{\"subject\":\"" + alert_subject + "\",\"body\":\"" + alert_body + "\"}";
    int alertCode = http.POST(jsonPayload);
    Serial.printf("Alert HTTP POST: %d\n", alertCode);
    http.end();
  }

  delay(20000); // 20-second delay
}