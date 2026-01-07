#include <cassert>
#include <iostream>
#include <thread>

#include "kv_store/kv_store.h"

using namespace kv_store;

int main() {
    // =========================
    // TTL Eviction Tests
    // =========================
    {
        KVStoreOptions options;
        options.num_shards = 16;
        options.eviction = EvictionType::TTL;

        KVStore store(options);

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

        // Test 5: TTL expiration
        store.put("temp_key", "temp_value", std::chrono::seconds(2));
        auto [found5, value5] = store.get("temp_key");
        assert(found5);
        assert(value5 == "temp_value");

        std::this_thread::sleep_for(std::chrono::seconds(3));  // wait for expiration
        auto [found6, value6] = store.get("temp_key");
        assert(!found6);
    }

    std::cout << "TTL eviction tests passed!\n";

    {
        // Test 6: basic LRU eviction
        KVStoreOptions options;
        options.num_shards = 4;
        options.eviction = EvictionType::LRU;
        options.lru_capacity = 2;

        KVStore lruStore(options);

        lruStore.put("a", "1", std::chrono::seconds(0));
        lruStore.put("b", "2", std::chrono::seconds(0));

        auto [fa, va] = lruStore.get("a");
        assert(fa);
        assert(va == "1");

        lruStore.put("c", "3", std::chrono::seconds(0));

        auto [fb, vb] = lruStore.get("b");
        assert(!fb);

        auto [fa2, va2] = lruStore.get("a");
        assert(fa2 && va2 == "1");

        auto [fc, vc] = lruStore.get("c");
        assert(fc && vc == "3");
    }

    {
        // Test 7: overwrite updates LRU order
        KVStoreOptions options;
        options.num_shards = 4;
        options.eviction = EvictionType::LRU;
        options.lru_capacity = 2;

        KVStore lruStore(options);

        lruStore.put("x", "1", std::chrono::seconds(0));
        lruStore.put("y", "2", std::chrono::seconds(0));

        lruStore.put("x", "10", std::chrono::seconds(0));

        lruStore.put("z", "3", std::chrono::seconds(0));

        auto [fy, vy] = lruStore.get("y");
        assert(!fy);

        auto [fx, vx] = lruStore.get("x");
        assert(fx && vx == "10");

        auto [fz, vz] = lruStore.get("z");
        assert(fz && vz == "3");
    }

    std::cout << "LRU eviction tests passed!\n";
    std::cout << "All single-threaded tests passed!\n";

    return 0;
}
