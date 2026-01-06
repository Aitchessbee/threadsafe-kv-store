#include <cassert>
#include <iostream>
#include <thread>

#include "kv_store/eviction/eviction_types.h"
#include "kv_store/kv_store.h"

using namespace kv_store;

int main() {
    KVStore store(16, EvictionType::TTL);

    // Test 1: put() and get()
    store.put("key1", "value1", std::chrono::seconds(0));  // 0 = no expiration
    auto [found1, value1] = store.get("key1");
    assert(found1);
    assert(value1 == "value1");

    // Test 2: get() for missing key
    auto [found2, value2] = store.get("missing");
    assert(!found2);
    assert(value2 == "");

    // Test 3: overwrite existing key
    store.put("key1", "value2", std::chrono::seconds(0));
    auto [found3, value3] = store.get("key1");
    assert(found3);
    assert(value3 == "value2");

    // Test 4: erase() key
    store.erase("key1");
    auto [found4, value4] = store.get("key1");
    assert(!found4);

    // Optional Test 5: TTL expiration
    store.put("temp_key", "temp_value", std::chrono::seconds(2));
    auto [found5, value5] = store.get("temp_key");
    assert(found5);

    std::this_thread::sleep_for(std::chrono::seconds(3));  // wait for expiration
    auto [found6, value6] = store.get("temp_key");
    assert(!found6);

    std::cout << "All single-threaded tests passed!\n";

    return 0;
}
