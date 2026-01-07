#pragma once

#include <atomic>
#include <optional>
#include <thread>

#include "../kv_store.h"
#include "eviction_policy.h"

namespace kv_store {

class TTLEviction : public EvictionPolicy {
   public:
    explicit TTLEviction(KVStore& store, std::chrono::milliseconds ttl_interval);
    ~TTLEviction() override;

    void start() override;

    void stop() override;

    void onGet(const std::string&) override {
    }

    std::optional<std::string> onPut(const std::string&) override {
        return std::nullopt;
    }

    void onErase(const std::string&) override {
    }

   private:
    void backgroundThreadFunc();

    KVStore& store_;
    std::atomic<bool> running_{false};
    std::chrono::milliseconds ttl_interval_;
    std::thread backgroundThread_;
};

}  // namespace kv_store
