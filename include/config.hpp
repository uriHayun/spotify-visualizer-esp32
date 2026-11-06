#pragma once

#include <cstddef>

namespace Config {

constexpr std::size_t MAX_RECONN_ATTEMPTS = 20;

// WiFi reconnecting time-period interval (ms)
constexpr long RECONN_INTERVAL_MS = 5000;

}  // namespace Config