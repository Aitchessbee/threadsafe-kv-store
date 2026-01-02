#include "../include/kv_store/kv_store.h"
#include <functional>

namespace kv_store {

KVStore::KVStore(size_t num_shards)
    : num_shards_(num_shards), shards_(num_shards) {}

size_t KVStore::getShardIndex(const std::string& key) const {
    std::hash<std::string> hasher;
    return hasher(key) % num_shards_;
}

void KVStore::put(const std::string& key, const std::string& value) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    std::unique_lock<std::shared_mutex> lock(shard.mutex);
    shard.map[key] = value;
}

std::pair<bool, std::string> KVStore::get(const std::string& key) const {
    size_t shard_index = getShardIndex(key);
    const Shard& shard = shards_[shard_index];

    std::shared_lock<std::shared_mutex> lock(shard.mutex);
    auto it = shard.map.find(key);
    if (it == shard.map.end()) return {false, ""};
    return {true, it->second};
}

void KVStore::erase(const std::string& key) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    std::unique_lock<std::shared_mutex> lock(shard.mutex);
    shard.map.erase(key);
}

}