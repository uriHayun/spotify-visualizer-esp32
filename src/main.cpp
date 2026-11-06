#include "config.hpp"
#include "secrets.hpp"

#include <Arduino.h>
#include <cstddef>
#include <WiFi.h>

unsigned long prev_ms = 0;

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        delay(10);
    }

    Serial.println("System initialized. Start Spotify visulizer...");

    init_wifi();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long curr_ms = millis();
    
        if (curr_ms - prev_ms >= Config::RECONN_INTERVAL_MS) {
            Serial.println("WiFi lost. Attempting to reconnect...");
            WiFi.reconnect();
        }
    }

    delay(100);
}

namespace Network {

void init_wifi() {
    // Set WiFi mode to STA (Station) - wireless microcontroller acts as a client
    WiFi.mode(WIFI_STA);

    // Scan for nearby networks and print to Serial Monitor
    const int NETWORKS = WiFi.scanNetworks();
    Serial.print("Found ");
    Serial.print(NETWORKS);
    Serial.println(" networks:");
    for (int i = 0; i < NETWORKS; i++) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.println(" dBm)");
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    std::size_t conn_attempts = 0;
    while (WiFi.status() != WL_CONNECTED && conn_attempts < Config::MAX_RECONN_ATTEMPTS) {
        delay(500);
        Serial.print(".");
        conn_attempts++;
    }

    // Move to a new line after the dots
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected successfully!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}

}  // namespace Network