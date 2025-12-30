#include <string>
#include "../include/kv_store/kv_store.h"

namespace kv_store {
    void KVStore::put(const std::string &key, const std::string &value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        store_[key] = value;
    }

    std::pair<bool, std::string> KVStore::get(const std::string &key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = store_.find(key);

        if(it == store_.end()) {
            return {false, ""};
        }

        return {true, it->second};
    }

    void KVStore::erase(const std::string &key) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        
        store_.erase(key);
    }
}