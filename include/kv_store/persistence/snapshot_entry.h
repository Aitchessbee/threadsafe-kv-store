#pragma once
#include <chrono>
#include <string>

namespace kv_store {

struct SnapshotEntry {
    std::string key;
    std::string value;
    std::chrono::system_clock::time_point expires_at;
};

}  // namespace kv_store
