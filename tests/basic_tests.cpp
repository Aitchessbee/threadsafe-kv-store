#include "../include/kv_store/kv_store.h"
#include <cassert>
#include <iostream>

using namespace kv_store;

int main() {
    KVStore store;

    // Test 1: put() and get()
    store.put("key1", "value1");
    auto [found1, value1] = store.get("key1");
    assert(found1);
    assert(value1 == "value1");

    // Test 2: get() for missing key
    auto [found2, value2] = store.get("missing");
    assert(!found2);
    assert(value2 == "");

    // Test 3: overwrite existing key
    store.put("key1", "value2");
    auto [found3, value3] = store.get("key1");
    assert(found3);
    assert(value3 == "value2");

    // Test 4: erase() key
    store.erase("key1");
    auto [found4, value4] = store.get("key1");
    assert(!found4);

    std::cout << "All single-threaded tests passed!\n";
    return 0;
}
