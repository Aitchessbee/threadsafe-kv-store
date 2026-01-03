#include "../include/kv_store/kv_store.h"

#include <functional>

namespace kv_store {

KVStore::KVStore(size_t num_shards) : num_shards_(num_shards), shards_(num_shards) {}

size_t KVStore::getShardIndex(const std::string& key) const {
    static std::hash<std::string> hasher;
    return hasher(key) % num_shards_;
}

void KVStore::put(const std::string& key, const std::string& value, std::chrono::steady_clock::time_point expire_at) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    ValueEntry entry{value, expire_at};

    std::unique_lock lock(shard.mutex);
    shard.map[key] = std::move(entry);
}

void KVStore::put(const std::string& key, const std::string& value, std::chrono::seconds ttl) {
    const auto expire_at = (ttl.count() > 0) ? std::chrono::steady_clock::now() + ttl : std::chrono::steady_clock::time_point::max();

    put(key, value, expire_at);
}

std::pair<bool, std::string> KVStore::get(const std::string& key) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    const auto now = std::chrono::steady_clock::now();

    {
        std::shared_lock lock(shard.mutex);
        auto it = shard.map.find(key);

        if (it == shard.map.end()) {
            return {false, ""};
        }

        if (now <= it->second.expire_at) {
            return {true, it->second.value};
        }
    }

    {
        std::unique_lock ulock(shard.mutex);
        auto it = shard.map.find(key);

        if (it == shard.map.end()) {
            return {false, ""};
        }

        if (now > it->second.expire_at) {
            shard.map.erase(it);
            return {false, ""};
        } else {
            return {true, it->second.value};
        }
    }
}

void KVStore::erase(const std::string& key) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    std::unique_lock lock(shard.mutex);
    shard.map.erase(key);
}

}  // namespace kv_store
