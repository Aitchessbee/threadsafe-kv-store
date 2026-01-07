#pragma once

#include <optional>

#include "eviction_policy.h"

namespace kv_store {

class NoEviction : public EvictionPolicy {
   public:
    void start() override {};
    void stop() override {};
    void onGet(const std::string&) override {};
    std::optional<std::string> onPut(const std::string&) override {
        return std::nullopt;
    };
    void onErase(const std::string&) override {};
};

}  // namespace kv_store