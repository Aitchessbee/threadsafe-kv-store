#include "../include/kv_store/kv_store.h"

#include <functional>

#include "kv_store/eviction/eviction_policy.h"
#include "kv_store/eviction/lru_eviction.h"
#include "kv_store/eviction/no_eviction.h"
#include "kv_store/eviction/ttl_eviction.h"

namespace kv_store {

KVStore::KVStore(const KVStoreOptions& options) : num_shards_(options.num_shards), shards_(options.num_shards) {
    switch (options.eviction) {
        case EvictionType::None:
            evictionPolicy_ = std::make_unique<NoEviction>();
            break;
        case EvictionType::TTL:
            evictionPolicy_ = std::make_unique<TTLEviction>(*this, options.ttl_scan_interval);
            break;
        case EvictionType::LRU:
            evictionPolicy_ = std::make_unique<LRUEviction>(options.lru_capacity);
            break;
        default:
            throw std::invalid_argument("Unknown eviction type");
    }

    evictionPolicy_->start();

    snapshotManager_ = std::make_unique<SnapshotManager>(options.snapshot_path);
}

size_t KVStore::getShardIndex(const std::string& key) const noexcept {
    static std::hash<std::string> hasher;
    return hasher(key) % num_shards_;
}

void KVStore::put(const std::string& key, const std::string& value, std::chrono::system_clock::time_point expire_at) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    ValueEntry entry{value, expire_at};

    {
        std::unique_lock lock(shard.mutex);
        shard.map[key] = std::move(entry);
    }

    std::optional<std::string> keyToRemove = evictionPolicy_->onPut(key);

    if (keyToRemove.has_value()) {
        erase(keyToRemove.value());
    }
}

void KVStore::put(const std::string& key, const std::string& value, std::chrono::seconds ttl) {
    const auto expire_at = (ttl.count() > 0) ? std::chrono::system_clock::now() + ttl : std::chrono::system_clock::time_point::max();

    put(key, value, expire_at);
}

std::pair<bool, std::string> KVStore::get(const std::string& key) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];
    const auto now = std::chrono::system_clock::now();

    std::string value;
    bool hit = false;

    {
        std::shared_lock lock(shard.mutex);
        auto it = shard.map.find(key);
        if (it != shard.map.end() && now <= it->second.expire_at) {
            value = it->second.value;
            hit = true;
        }
    }

    if (hit) {
        evictionPolicy_->onGet(key);
        return {true, value};
    }

    bool erased = false;

    {
        std::unique_lock lock(shard.mutex);
        auto it = shard.map.find(key);
        if (it != shard.map.end() && now > it->second.expire_at) {
            shard.map.erase(it);
            erased = true;
        }
    }

    if (erased) {
        evictionPolicy_->onErase(key);
    }

    return {false, ""};
}

void KVStore::erase(const std::string& key) {
    size_t shard_index = getShardIndex(key);
    Shard& shard = shards_[shard_index];

    bool erased = false;

    {
        std::unique_lock lock(shard.mutex);
        erased = (shard.map.erase(key) > 0);
    }

    if (erased) {
        evictionPolicy_->onErase(key);
    }
}

void KVStore::removeExpiredKeys() {
    const auto now = std::chrono::system_clock::now();

    for (auto& shard : shards_) {
        std::vector<std::string> expired;

        {
            std::unique_lock lock(shard.mutex);
            for (auto it = shard.map.begin(); it != shard.map.end();) {
                if (it->second.expire_at < now) {
                    expired.push_back(it->first);
                    it = shard.map.erase(it);
                } else {
                    ++it;
                }
            }
        }

        for (const auto& key : expired) {
            evictionPolicy_->onErase(key);
        }
    }
}

void KVStore::saveSnapshot() {
    std::vector<SnapshotEntry> entries;

    for (auto& shard : shards_) {
        std::shared_lock lock(shard.mutex);

        for (const auto& [key, valueEntry] : shard.map) {
            entries.emplace_back(key, valueEntry.value, valueEntry.expire_at);
        }
    }

    snapshotManager_->saveSnapshot(entries);
}

void KVStore::loadSnapshot() {
    std::vector<SnapshotEntry> entries = snapshotManager_->loadSnapshot();

    for (auto& entry : entries) {
        put(entry.key, entry.value, entry.expires_at);
    }
}

}  // namespace kv_store
