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

- **Persistence Support**
  
  - Save and load the store state to/from binary snapshots for data durability across restarts.

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
cmake -S . -B build
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

### Persistence (Snapshot) Example

```cpp
KVStoreOptions options;
options.snapshot_path = "store.bin";

KVStore store(options);
store.put("session_id", "xyz123", std::chrono::hours(1));

store.saveSnapshot();

store.loadSnapshot();

```

---

### Interactive CLI

The project includes a CLI for manual interaction:

```bash
./build/kv_store_cli
> put mykey myvalue 60
OK
> get mykey
"myvalue"
> save
Snapshot saved to store.bin

```

---

## API Overview

| Method                 | Description                                                    |
| ---------------------- | -------------------------------------------------------------- |
| `put(key, value, ttl)` | Inserts or updates a key with optional TTL                     |
| `get(key)`             | Retrieves a key, updating recency for LRU                      |
| `erase(key)`           | Removes a key from the store                                   |
| `saveSnapshot()`       | Persists current data to a binary file                         |
| `loadSnapshot()`       | Restores data from the snapshot file                           |
| `removeExpiredKeys()`  | Manually clean up expired keys                                 |
| `KVStoreOptions`       | Configure shards, eviction, snapshot path, and capacity limits |

---

## Thread Safety

- `put`, `get`, and `erase` are fully **thread-safe**
- Shard-level locks prevent contention, allowing multiple threads to access **different shards concurrently**
- `saveSnapshot` uses shared locks to allow concurrent reads while serializing data
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
- Concurrent snapshotting during active writes is validated in `mt_tests`

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
3. **Binary Serialization**: `SnapshotManager` uses a length-prefixed binary format for efficient disk I/O.
4. **Optional TTL**: Keys can expire automatically or be cleaned up manually.
5. **Optional LRU**: Keys are evicted based on recency when capacity is reached.
6. **Thread-Safe Eviction**: `onPut`, `onGet`, and `onErase` are safe across threads.

---

## Contributing

Contributions are welcome!

- Add new eviction policies
- Improve performance benchmarks
- Improve multithreaded stress tests

Please fork the repo and submit a pull request.
