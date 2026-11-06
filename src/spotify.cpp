#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

namespace Spotify {

void fetch_curr_track() {
    WiFiClientSecure client;
    HTTPClient http;

    Serial.println("Fetching  playing track from Spotify...");

    if (https.begin(client, "https://api.spotify.com/v1/me/player/currently-playing")) {
    }
}

void update_curr_track() {
    if (WiFi.status() == WL_CONNECTED) {
        unsigned long current_millis = millis();
        if (current_millis - last_spotify_poll >= SPOTIFY_POLL_INTERVAL) {
            last_spotify_poll = current_millis;
            Spotify::fetch_curr_track();
        }
    }
}

}  // namespace Spotify
