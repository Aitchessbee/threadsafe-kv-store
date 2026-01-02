#include "../include/kv_store/kv_store.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace kv_store;
using Clock = std::chrono::high_resolution_clock;

double ops_per_sec(size_t ops, std::chrono::milliseconds duration) {
    if (duration.count() == 0) return 0.0;
    return static_cast<double>(ops) / (duration.count() / 1000.0);
}

void benchmark_put(KVStore& store, int num_threads, int ops_per_thread) {
    auto start = Clock::now();

    auto writer = [&](int tid) {
        for (int i = 0; i < ops_per_thread; ++i) {
            store.put(
                "key_" + std::to_string(tid) + "_" + std::to_string(i),
                "value_" + std::to_string(i)
            );
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(writer, i);
    }

    for (auto& t : threads) t.join();

    auto end = Clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    size_t total_ops = static_cast<size_t>(num_threads) * ops_per_thread;

    std::cout << "[PUT]   Threads=" << num_threads
              << " Ops=" << total_ops
              << " Time=" << duration.count() << "ms"
              << " Throughput=" << ops_per_sec(total_ops, duration)
              << " ops/sec\n";
}

void benchmark_get(KVStore& store, int num_threads, int ops_per_thread) {
    auto start = Clock::now();

    auto reader = [&](int tid) {
        for (int i = 0; i < ops_per_thread; ++i) {
            store.get(
                "key_" + std::to_string(tid) + "_" + std::to_string(i)
            );
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(reader, i);
    }

    for (auto& t : threads) t.join();

    auto end = Clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    size_t total_ops = static_cast<size_t>(num_threads) * ops_per_thread;

    std::cout << "[GET]   Threads=" << num_threads
              << " Ops=" << total_ops
              << " Time=" << duration.count() << "ms"
              << " Throughput=" << ops_per_sec(total_ops, duration)
              << " ops/sec\n";
}

void benchmark_mixed(KVStore& store,
                     int num_readers,
                     int num_writers,
                     int ops_per_thread) {
    auto start = Clock::now();

    auto reader = [&](int tid) {
        for (int i = 0; i < ops_per_thread; ++i) {
            store.get(
                "key_" + std::to_string(tid) + "_" + std::to_string(i)
            );
        }
    };

    auto writer = [&](int tid) {
        for (int i = 0; i < ops_per_thread; ++i) {
            store.put(
                "mixed_key_" + std::to_string(tid) + "_" + std::to_string(i),
                "value"
            );
        }
    };

    std::vector<std::thread> threads;

    for (int i = 0; i < num_readers; ++i)
        threads.emplace_back(reader, i);

    for (int i = 0; i < num_writers; ++i)
        threads.emplace_back(writer, i);

    for (auto& t : threads) t.join();

    auto end = Clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    size_t total_ops =
        static_cast<size_t>(num_readers + num_writers) * ops_per_thread;

    std::cout << "[MIXED] Readers=" << num_readers
              << " Writers=" << num_writers
              << " Ops=" << total_ops
              << " Time=" << duration.count() << "ms"
              << " Throughput=" << ops_per_sec(total_ops, duration)
              << " ops/sec\n";
}

int main() {
    const int shards = 16;
    const int threads = 8;
    const int ops_per_thread = 100000;

    std::cout << "KVStore Benchmark\n";
    std::cout << "Shards=" << shards
              << " Threads=" << threads
              << " Ops/thread=" << ops_per_thread << "\n\n";

    KVStore store(shards);

    benchmark_put(store, threads, ops_per_thread);
    benchmark_get(store, threads, ops_per_thread);
    benchmark_mixed(store, 6, 2, ops_per_thread);

    return 0;
}
