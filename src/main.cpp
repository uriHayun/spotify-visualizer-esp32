#include "network.hpp"
#include "spotify.hpp"

#include <Arduino.h>
#include <WiFi.h>

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        delay(10);
    }

    Serial.println("System initialized. Start Spotify visulizer...");

    Network::init_wifi();

    // Sync clock via NTP so TLS certificate validation succeeds (prevents 1970 clock default error)
    Spotify::Detail::ntp_sync_time();

    Spotify::fetch_curr_track();
}

void loop() {
    // Continuosly check WiFi connection and attempt reconnection if lost
    Network::handle_reconn();

    // Continuosly update currently playing track if connected to WiFi
    Spotify::update_curr_track();

    delay(100);
}
