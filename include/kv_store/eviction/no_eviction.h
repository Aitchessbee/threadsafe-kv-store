#pragma once

#include "eviction_policy.h"

namespace kv_store {

class NoEviction : public EvictionPolicy {
   public:
    void start() override {};
    void stop() override {};
    void onGet(const std::string&) override {};
    void onPut(const std::string&) override {};
    void onErase(const std::string&) override {};
};

}  // namespace kv_store