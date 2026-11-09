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

    // Sync clock via NTP so TLS certificate validation succeeds
    //(prevent 1970 Unix Epoch default time problem)
    if (Spotify::Detail::sync_device_ntp()) {
        Serial.println("NTP time sync successful");
    } else {
        Serial.println("NTP time sync failed. TLS certificate validation might fail");
    }

    TrackInfo curr_track_info = Spotify::fetch_curr_track_info();
}

void loop() {
    // Continuosly check WiFi connection and attempt reconnection if lost
    Network::handle_reconn();

    // Continuosly update currently playing track if connected to WiFi
    Spotify::update_curr_track();

    delay(100);
}
