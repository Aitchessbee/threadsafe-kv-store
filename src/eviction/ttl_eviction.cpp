#include "kv_store/eviction/ttl_eviction.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace kv_store {

TTLEviction::TTLEviction(KVStore& store) : store_(store), running_(false) {
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
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

}  // namespace kv_store
