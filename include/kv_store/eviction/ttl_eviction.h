#pragma once

#include <atomic>
#include <thread>

#include "../kv_store.h"
#include "eviction_policy.h"

namespace kv_store {

class TTLEviction : public EvictionPolicy {
   public:
    explicit TTLEviction(KVStore& store);
    ~TTLEviction() override;

    void start() override;
    void stop() override;

    void onGet(const std::string&) override {
    }
    void onPut(const std::string&) override {
    }
    void onErase(const std::string&) override {
    }

   private:
    void backgroundThreadFunc();

    KVStore& store_;
    std::atomic<bool> running_{false};
    std::thread backgroundThread_;
};

}  // namespace kv_store
