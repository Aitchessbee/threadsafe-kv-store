#pragma once

#include <list>
#include <mutex>

#include "../kv_store.h"
#include "eviction_policy.h"
#include "unordered_map"

namespace kv_store {

class LRUEviction : public EvictionPolicy {
   public:
    explicit LRUEviction(int capacity = 10);
    ~LRUEviction() override;

    void start() override {};
    void stop() override {};
    void onGet(const std::string&) override;
    std::optional<std::string> onPut(const std::string&) override;
    void onErase(const std::string&) override;

   private:
    std::list<std::string> list_;
    std::unordered_map<std::string, std::list<std::string>::iterator> keyToIteratorMap_;
    int capacity_;
    std::mutex mutex_;
};

}  // namespace kv_store