# Threadsafe In-Memory Key-Value Store

A **high-performance, multithreaded in-memory key-value store** with pluggable eviction policies, written in modern C++. It supports **sharded storage**, **TTL-based expiration**, and **LRU eviction**, with full thread safety for concurrent access.

---

## Features

- **Thread-Safe**
  Fully concurrent `put`, `get`, and `erase` operations using per-shard locks.

- **Sharded Storage**
  Keys are distributed across multiple shards for high concurrency and low lock contention.

- **Pluggable Eviction Policies**

  - **None**: No eviction, store grows until memory limit
  - **TTL**: Time-to-live expiration for keys
  - **LRU**: Least Recently Used eviction with configurable capacity

- **Flexible Key Expiration**

  - TTL-based expiration per key
  - Background cleanup of expired keys for TTL policy

- **Simple and Extensible API**

  - Easy to add new eviction policies
  - Fully configurable via `KVStoreOptions`

---

## Installation

Clone the repository:

```bash
git clone https://github.com/Aitchessbee/threadsafe-kv-store.git
cd threadsafe-kv-store
```

Build using CMake (requires C++20 or later):

```bash
cmake -S . -B
cmake --build build
```

---

## Usage

### Basic KVStore

```cpp
#include "kv_store/kv_store.h"
#include <chrono>
#include <iostream>

using namespace kv_store;

int main() {
    KVStoreOptions options;
    options.num_shards = 16;
    options.eviction = EvictionType::None;

    KVStore store(options);

    store.put("key1", "value1", std::chrono::seconds(0));
    auto [found, value] = store.get("key1");

    if (found) {
        std::cout << "Key1: " << value << "\n";
    }
}
```

---

### LRU Eviction Example

```cpp
KVStoreOptions options;
options.num_shards = 4;
options.eviction = EvictionType::LRU;
options.lru_capacity = 100;  // maximum 100 keys

KVStore store(options);

store.put("a", "1", std::chrono::seconds(0));
store.put("b", "2", std::chrono::seconds(0));

// Access "a" to make it most recently used
store.get("a");

// Insert "c", "b" may get evicted if capacity exceeded
store.put("c", "3", std::chrono::seconds(0));
```

---

### TTL Eviction Example

```cpp
KVStoreOptions options;
options.num_shards = 8;
options.eviction = EvictionType::TTL;
options.ttl_scan_interval = std::chrono::milliseconds(500);

KVStore store(options);

// Key expires after 2 seconds
store.put("temp", "value", std::chrono::seconds(2));

// Key never expires if TTL = 0
store.put("permanent", "value", std::chrono::seconds(0));
```

---

## API Overview

| Method                 | Description                                                             |
| ---------------------- | ----------------------------------------------------------------------- |
| `put(key, value, ttl)` | Inserts or updates a key with optional TTL                              |
| `get(key)`             | Retrieves a key, updating recency for LRU                               |
| `erase(key)`           | Removes a key from the store                                            |
| `removeExpiredKeys()`  | Manually clean up expired keys                                          |
| `KVStoreOptions`       | Configure number of shards, eviction policy, TTL interval, LRU capacity |

---

## Thread Safety

- `put`, `get`, and `erase` are fully **thread-safe**
- Shard-level locks prevent contention, allowing multiple threads to access **different shards concurrently**
- Eviction notifications happen **outside shard locks** to avoid deadlocks

---

## Testing

Run the included **unit tests**:

```bash
cmake --build build
./build/basic_tests
./build/mt_tests
```

- Single-threaded TTL and LRU behavior are validated
- Multi-threaded access is safe and deadlock-free

---

## Benchmarking

Measure KVStore performance using the provided benchmark:

```bash
cmake --build build
./build/benchmark
```

### Workloads

- **PUT-only**: concurrent inserts
- **GET-only**: concurrent reads
- **Mixed**: readers and writers together

### Example Output

```
[PUT]   Threads=8 Ops=800000 Time=120ms Throughput=6.6M ops/sec
[GET]   Threads=8 Ops=800000 Time=90ms  Throughput=8.8M ops/sec
[MIXED] Readers=6 Writers=2 Ops=800000 Time=150ms Throughput=5.3M ops/sec
```

### Configurable Parameters

- `shards`, `threads`, `ops_per_thread`
- Eviction policy via `KVStoreOptions.eviction` (`LRU`, `TTL`, `None`)
- LRU capacity or TTL scan interval

> Higher throughput = lower contention and better concurrency.

---

## Design Highlights

1. **Sharded Storage**: Each shard has its own lock, reducing contention.
2. **Eviction Policies**: Encapsulated as pluggable classes (`NoEviction`, `LRUEviction`, `TTLEviction`).
3. **Optional TTL**: Keys can expire automatically or be cleaned up manually.
4. **Optional LRU**: Keys are evicted based on recency when capacity is reached.
5. **Thread-Safe Eviction**: `onPut`, `onGet`, and `onErase` are safe across threads.

---

## Contributing

Contributions are welcome!

- Add new eviction policies
- Add performance benchmarks
- Improve multithreaded stress tests

Please fork the repo and submit a pull request.
