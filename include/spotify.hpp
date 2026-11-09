#include <cstddef>
#include <string_view>

namespace Spotify {

struct TrackInfo {
    String title;
    String artist;
    String album;
    bool is_playing = false;
    bool success = false;
};

TrackInfo fetch_curr_track_info();

void update_curr_track();

}  // namespace Spotify

namespace Spotify::Detail {

constexpr std::string_view NTP_SERVER = "pool.ntp.org";
constexpr std::size_t TIMEOUT_MS = 20000;
constexpr std::size_t minimum_valid_epoch = 1767225600;  // 2026-01-01 00:00:00 UTC

bool sync_device_ntp();

}  // namespace Spotify::Detail
