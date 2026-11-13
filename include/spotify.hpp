#include <cstddef>
#include <string_view>

#include <Arduino.h>

namespace Spotify {

struct TrackInfo {
    String title;
    String artist;
    String album;
    bool is_playing = false;
    bool success = false;
};

TrackInfo fetch_curr_track();

void update_curr_track(TrackInfo &current_track);

}  // namespace Spotify

namespace Spotify::Detail {

inline constexpr std::string_view NTP_SERVER = "pool.ntp.org";
inline constexpr std::size_t TIMEOUT_MS = 20000;
inline constexpr std::size_t minimum_valid_epoch = 1767225600;  // 2026-01-01 00:00:00 UTC

bool sync_device_ntp();

bool tls_certify_requests(WiFiClientSecure &client);

}  // namespace Spotify::Detail
