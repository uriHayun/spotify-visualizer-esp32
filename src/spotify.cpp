#include "spotify.hpp"
#include "secrets.hpp"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <string>

namespace Spotify {

void fetch_curr_track() {
    WiFiClientSecure client;
    HTTPClient http;

    Serial.println("Fetching  playing track from Spotify...");

    if (!http.begin(client, "https://api.spotify.com/v1/me/player/currently-playing")) {
        Serial.println("Failed to begin HTTPS session");
        return;
    }

    String auth_header = "Bearer " + String(SPOTIFY_ACCESS_TOKEN);

    http.addHeader("Authorization", auth_header);
    http.addHeader("Content-Type", "application/json");

    const int http_res_code = http.GET();

    if (http_res_code >= 0) {
        switch (http_res_code) {
        case (HTTP_CODE_OK):
            const String payload = http.getString().c_str();
            Serial.println("Received response from Spotify API: ", payload.c_str());

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
                Serial.print("Failed to parse JSON: ");
                Serial.println(error.c_str());
                http.end();
                return;
            }

            bool is_playing = doc["is_playing"] | false;

            if (!doc["item"].isNull()) {
                const String track_name = doc["item"]["name"];
                const String artist_name = doc["item"]["artists"][0]["name"];
                const String album_name = doc["item"]["album"]["name"];
            } else {
                Serial.println("There was no active playback found (track data is NULL)");
            }

            break;
        case (HTTP_CODE_NO_CONTENT):
            Serial.println("HTTP 204: no content. Connected successfully, no track is playing "
                           "right now though");
            break;
        case (HTTP_CODE_UNAUTHORIZED):
            Serial.println("HTTP 401: unauthorized. "
                           "Your Spotify access token has expired or is invalid");
            break;
        case (HTTP_CODE_FORBIDDEN):
            Serial.println("HTTP 403: access forbidden. Your Spotify access token has expired, is "
                           "invalid, or your account isn't premium");
            break;
        case (HTTP_CODE_TOO_MANY_REQUESTS):
            Serial.println("HTTP 429: too many requests. Application has exceeded rate limits");
            break;
        default:
            Serial.printf("Unexpected HTTP response code: %d\n", http_res_code);
            break;
        }
    }

    http.end();
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

namespace Spotify::Detail {

void ntp_sync_time() {
    // Configuration for standard UTC time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Waiting for NTP time sync: ");
    time_t now_secs = time(nullptr);

    // Wait until year is past 1970 (~9.1.1970)
    while (now_secs < 8 * 3600 * 24) {
        delay(500);
        Serial.print(".");
        now_secs = time(nullptr);
    }

    Serial.println();
    Serial.println("Time synchronized successfully!");
}

}  // namespace Spotify::Detail
