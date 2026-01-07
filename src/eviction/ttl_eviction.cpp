#include "kv_store/eviction/ttl_eviction.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace kv_store {

TTLEviction::TTLEviction(KVStore& store, std::chrono::milliseconds ttl_interval) : store_(store), running_(false), ttl_interval_(ttl_interval) {
}

TTLEviction::~TTLEviction() {
    stop();
}

void TTLEviction::start() {
    if (running_.exchange(true)) {
        return;
    }

    backgroundThread_ = std::thread(&TTLEviction::backgroundThreadFunc, this);
}

void TTLEviction::stop() {
    if (!running_.exchange(false)) {
        return;
    }

    if (backgroundThread_.joinable()) {
        backgroundThread_.join();
    }
}

void TTLEviction::backgroundThreadFunc() {
    while (running_) {
        store_.removeExpiredKeys();
        std::this_thread::sleep_for(std::chrono::milliseconds(ttl_interval_));
    }
}

}  // namespace kv_store
