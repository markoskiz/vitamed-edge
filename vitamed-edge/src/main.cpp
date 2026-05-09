#include <Arduino.h>
#include "HX711.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <time.h>

#define DOUT 5
#define CLK 4
#define SCALE_URL "https://api.vitamed.mk/api/readings"
#define UNIT_ID "vtmd-a-001"
#define DEPLOYMENT_ID "susica"
#define WIFI_SSID "Telekom-498264"
#define WIFI_PASS "91117151"

HX711 scale;
HTTPClient http;

void setup() {
    Serial.begin(115200);
    delay(3000);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    configTime(0, 0, "pool.ntp.org");

    scale.begin(DOUT, CLK);
    scale.tare();
    scale.set_scale(20.68);
    Serial.println("Scale ready!");
}

void loop() {
    if (scale.is_ready()) {
        float grams = scale.get_units(10);
        Serial.print("Weight: ");
        Serial.print(grams, 1);
        Serial.println(" g");

        time_t now;
        struct tm timeinfo;
        time(&now);
        gmtime_r(&now, &timeinfo);
        char timestamp[30];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S.000Z", &timeinfo);

        http.begin(SCALE_URL);
        http.addHeader("Content-Type", "application/json");
        String payload = "{\"unit_id\":\"" UNIT_ID "\","
                         "\"time_created\":\"" + String(timestamp) + "\","
                         "\"reading\":" + String(grams, 1) + ","
                         "\"deployment\":\"" DEPLOYMENT_ID "\"}";
        http.POST(payload);
        http.end();
    }
    delay(1000);
}
