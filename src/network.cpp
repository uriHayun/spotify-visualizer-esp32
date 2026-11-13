
#include "network.hpp"
#include "config.hpp"
#include "secrets.hpp"

#include <WiFi.h>

#include <cstddef>

namespace {

unsigned long prev_ms = 0;

}  // namespace

namespace Network {

void init_wifi() {
    // Set WiFi mode to STA (Station) - wireless microcontroller acts as a client
    WiFi.mode(WIFI_STA);

    // Scan for nearby networks and print to Serial Monitor
    const int NETWORKS = WiFi.scanNetworks();
    Serial.printf("Found %d networks: ", NETWORKS);
    for (int i = 0; i < NETWORKS; i++) {
        const String ssid = WiFi.SSID(i);
        Serial.printf("%d: %s (%d dBm)\n", i + 1, ssid.c_str(), WiFi.RSSI(i));
    }

    WiFi.begin(Secrets::Wifi::WIFI_SSID, Secrets::Wifi::WIFI_PASSWORD);

    std::size_t conn_attempts = 0;
    while (WiFi.status() != WL_CONNECTED && conn_attempts < Config::Wifi::MAX_RECONN_ATTEMPTS) {
        delay(500);
        Serial.print(".");
        conn_attempts++;
    }

    // Move to a new line after the dots
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        const String ip_address = WiFi.localIP().toString();
        Serial.printf("WiFi connected successfully!\nIP Address: %s\n", ip_address.c_str());
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}

bool handle_reconn() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long curr_ms = millis();

        if (curr_ms - prev_ms >= Config::Wifi::RECONN_INTERVAL_MS) {
            Serial.println("WiFi lost. Attempting to reconnect...");
            prev_ms = curr_ms;
            WiFi.reconnect();
        }
    }

    return WiFi.status() == WL_CONNECTED;
}

}  // namespace Network
