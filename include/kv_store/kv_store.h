#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <vector>
#include <utility>

namespace kv_store {

class KVStore {
public:
    explicit KVStore(size_t num_shards = 16);

    std::pair<bool, std::string> get(const std::string& key) const;
    void put(const std::string& key, const std::string& value);
    void erase(const std::string& key);

private:
    struct Shard {
        mutable std::shared_mutex mutex;
        std::unordered_map<std::string, std::string> map;
    };

    size_t num_shards_;
    std::vector<Shard> shards_;

    size_t getShardIndex(const std::string& key) const;
};

}
