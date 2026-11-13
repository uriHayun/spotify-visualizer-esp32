#include "spotify.hpp"
#include "esp_sntp.h"
#include "secrets.hpp"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <chrono>
#include <thread>

namespace Spotify {

TrackInfo fetch_curr_track_info() {
    TrackInfo track_info;
    WiFiClientSecure client;
    HTTPClient http;

    if (!Detail::tls_certify_requests(client)) {
        return track_info;
    }

    Serial.println("Fetching playing track from Spotify...");

    if (!http.begin(client, "https://api.spotify.com/v1/me/player/currently-playing")) {
        Serial.println("Failed to begin HTTPS session");
        return track_info;
    }

    String auth_header = "Bearer " + String(Secrets::Spotify::SPOTIFY_ACCESS_TOKEN);

    http.addHeader("Authorization", auth_header);
    http.addHeader("Content-Type", "application/json");

    const int http_res_code = http.GET();

    // Intercept low-level network or hardware errors first
    if (http_res_code <= 0) {
        Serial.printf("HTTP %d: connection failed or blocked\n", http_res_code);
        return track_info;
    }

    switch (http_res_code) {
    case (HTTP_CODE_OK): {
        const String payload = http.getString();
        Serial.printf("Received response from Spotify API: %s\n", payload.c_str());

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, payload);

        if (err) {
            Serial.printf("Failed to parse JSON: %s\n", err.c_str());
            http.end();
            return track_info;
        }

        track_info.is_playing = doc["is_playing"] | false;

        if (!doc["item"].isNull()) {
            track_info.title = doc["item"]["name"].as<String>();
            track_info.artist = doc["item"]["artists"][0]["name"].as<String>();
            track_info.album = doc["item"]["album"]["name"].as<String>();
            track_info.success = true;
        } else {
            Serial.println("There was no active playback found (track data is NULL)");
        }

        break;
    }

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

    http.end();
    return track_info;
}

void update_curr_track_info(TrackInfo &curr_track_info) {
    static unsigned long last_spotify_poll = 0;

    const unsigned long now_ms = millis();
    if (WiFi.status() == WL_CONNECTED &&
        now_ms - last_spotify_poll >= Config::Spotify::SPOTIFY_POLL_INTERVAL_MS) {
        last_spotify_poll = now_ms;
        curr_track_info = fetch_curr_track_info();
    }
}

}  // namespace Spotify

namespace Spotify::Detail {

// Solves Unix Epoch default time problem by syncing the time with NTP,
// in order to validate TLS certificates for HTTPS requests (prevents 1970 clock default error)
bool sync_device_ntp() {
    if (esp_sntp_enabled() == 0) {
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, Config::Spotify::NTP_SERVER.data());
        esp_sntp_init();
    }

    const auto start_time = std::chrono::steady_clock::now();

    Serial.print("Waiting for NTP time sync: ");
    while (true) {
        if (std::time(nullptr) >= Config::Spotify::MIN_VALID_EPOCH) {
            Serial.println();
            Serial.println("Time synchronized successfully!");

            return true;
        }

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time)
                                 .count();

        if (static_cast<std::size_t>(elapsed) >= Config::Spotify::NTP_SYNC_TIMEOUT_MS) {
            Serial.println();
            return false;
        }

        Serial.printf(".");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool tls_certify_requests(WiFiClientSecure &client) {
    if (Secrets::Spotify::SPOTIFY_ROOT_CA == nullptr ||
        Secrets::Spotify::SPOTIFY_ROOT_CA[0] == '\0') {
        Serial.println("Spotify root CA certificate isn't configured");
        return false;
    }

    client.setCACert(Secrets::Spotify::SPOTIFY_ROOT_CA);
    return true;
}

}  // namespace Spotify::Detail
