#pragma once

#include <chrono>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kv_store {

class KVStore {
   public:
    explicit KVStore(size_t num_shards = 16);

    std::pair<bool, std::string> get(const std::string& key);

    void put(const std::string& key, const std::string& value, std::chrono::steady_clock::time_point expire_at);

    void put(const std::string& key, const std::string& value, std::chrono::seconds ttl);

    void erase(const std::string& key);

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

    size_t getShardIndex(const std::string& key) const;
};

}  // namespace kv_store
