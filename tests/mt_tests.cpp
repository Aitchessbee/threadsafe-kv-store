#include "../include/kv_store/kv_store.h"

#include <thread>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>

using namespace kv_store;

// Test 1: Concurrent Writes
void test_concurrent_put() {
    KVStore store;

    const int num_threads = 8;
    const int writes_per_thread = 1000;

    auto writer = [&](int thread_id) {
        for (int i = 0; i < writes_per_thread; ++i) {
            store.put("key_" + std::to_string(i),
                      "value_from_thread_" + std::to_string(thread_id));
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(writer, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < writes_per_thread; ++i) {
        auto [found, value] = store.get("key_" + std::to_string(i));
        assert(found);
        assert(!value.empty());
    }

    std::cout << "✓ Concurrent put() test passed\n";
}

// Test 2: Concurrent Reads + Writes
void test_concurrent_get_and_put() {
    KVStore store;

    for (int i = 0; i < 1000; ++i) {
        store.put("key_" + std::to_string(i), "initial");
    }

    auto reader = [&]() {
        for (int i = 0; i < 1000; ++i) {
            store.get("key_" + std::to_string(i));
        }
    };

    auto writer = [&]() {
        for (int i = 0; i < 1000; ++i) {
            store.put("key_" + std::to_string(i), "updated");
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(reader);
        threads.emplace_back(writer);
    }

    for (auto& t : threads) {
        t.join();
    }

    // Invariant: keys still exist
    for (int i = 0; i < 1000; ++i) {
        auto [found, value] = store.get("key_" + std::to_string(i));
        assert(found);
    }

    std::cout << "✓ Concurrent get() + put() test passed\n";
}

// Test 3: Concurrent Erase + Access
void test_concurrent_erase() {
    KVStore store;

    // Pre-fill store
    for (int i = 0; i < 1000; ++i) {
        store.put("key_" + std::to_string(i), "value");
    }

    auto reader = [&]() {
        for (int i = 0; i < 1000; ++i) {
            store.get("key_" + std::to_string(i));
        }
    };

    auto eraser = [&]() {
        for (int i = 0; i < 1000; ++i) {
            store.erase("key_" + std::to_string(i));
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(reader);
        threads.emplace_back(eraser);
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < 1000; ++i) {
        store.get("key_" + std::to_string(i));
    }

    std::cout << "✓ Concurrent erase() test passed\n";
}

int main() {
    test_concurrent_put();
    test_concurrent_get_and_put();
    test_concurrent_erase();

    std::cout << "\nAll multithreaded tests passed successfully!\n";
    return 0;
}
