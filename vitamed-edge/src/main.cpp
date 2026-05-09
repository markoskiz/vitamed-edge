#include <Arduino.h>
#include "HX711.h"
#include <HTTPClient.h>
#include <WiFi.h>

#define DOUT        5
#define CLK         4
#define PI_IP       "192.168.1.50"          
#define SCALE_URL   "http://" PI_IP ":3000/api/readings"
#define UNIT_ID     "vtmd-a-001"
#define DEPLOYMENT  "susica"
#define WIFI_SSID   "Telekom-498264"
#define WIFI_PASS   "91117151"

HX711 scale;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  delay(3000);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");

  scale.begin(DOUT, CLK);
  scale.tare();
  scale.set_scale(20.68);
  Serial.println("Scale ready");
}

void loop() {
  if (scale.is_ready()) {
    float grams = scale.get_units(10);
    Serial.print("Weight: ");
    Serial.print(grams, 1);
    Serial.println(" g");

    String payload = "{\"unit_id\":\"" UNIT_ID "\","
                     "\"reading\":"  + String(grams, 1) + ","
                     "\"deployment\":\"" DEPLOYMENT "\"}";

    http.begin(SCALE_URL);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(payload);
    Serial.print("HTTP: ");
    Serial.println(code);
    http.end();
  }
  delay(1000);
}
