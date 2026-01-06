#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "eviction/eviction_policy.h"
#include "eviction/eviction_types.h"

namespace kv_store {

class KVStore {
   public:
    explicit KVStore(size_t num_shards = 16, EvictionType evictionType = EvictionType::None);

    std::pair<bool, std::string> get(const std::string& key);

    void put(const std::string& key, const std::string& value, std::chrono::steady_clock::time_point expire_at);

    // ttl <= 0 means the key never expires
    void put(const std::string& key, const std::string& value, std::chrono::seconds ttl);

    void erase(const std::string& key);

    void removeExpiredKeys();

   private:
    struct ValueEntry {
        std::string value;
        std::chrono::steady_clock::time_point expire_at;
    };

    struct Shard {
        mutable std::shared_mutex mutex;
        std::unordered_map<std::string, ValueEntry> map;
    };

    size_t num_shards_;
    std::vector<Shard> shards_;

    std::unique_ptr<EvictionPolicy> evictionPolicy_;

    size_t getShardIndex(const std::string& key) const noexcept;
};

}  // namespace kv_store
