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

TrackInfo fetch_curr_track_info();

void update_curr_track_info(TrackInfo &current_track);

}  // namespace Spotify

namespace Spotify::Detail {

bool sync_device_ntp();
bool tls_certify_requests(WiFiClientSecure &client);

}  // namespace Spotify::Detail
