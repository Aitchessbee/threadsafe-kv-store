#include "kv_store/eviction/lru_eviction.h"

#include "kv_store/kv_store.h"

namespace kv_store {

LRUEviction::LRUEviction(int capacity) : capacity_(capacity) {
}

LRUEviction::~LRUEviction() {
    stop();
}

std::optional<std::string> LRUEviction::onPut(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (capacity_ == 0)
        return std::nullopt;

    auto it = keyToIteratorMap_.find(key);
    if (it == keyToIteratorMap_.end()) {
        list_.push_back(key);
        keyToIteratorMap_[key] = std::prev(list_.end());

        if (list_.size() > static_cast<size_t>(capacity_)) {
            const std::string evicted = list_.front();
            list_.pop_front();
            keyToIteratorMap_.erase(evicted);
            return evicted;
        }
    } else {
        list_.erase(it->second);
        list_.push_back(key);
        it->second = std::prev(list_.end());
    }

    return std::nullopt;
}

void LRUEviction::onGet(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = keyToIteratorMap_.find(key);
    if (it == keyToIteratorMap_.end())
        return;

    list_.erase(it->second);
    list_.push_back(key);
    it->second = std::prev(list_.end());
}

void LRUEviction::onErase(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = keyToIteratorMap_.find(key);
    if (it == keyToIteratorMap_.end())
        return;

    list_.erase(it->second);
    keyToIteratorMap_.erase(it);
}

}  // namespace kv_store