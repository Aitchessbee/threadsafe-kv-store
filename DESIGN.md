# Design Doc

## Overview

This project implements a thread-safe, in-memory key-value store written in modern C++. It is designed to be used as a library within multi-threaded applications, allowing multiple threads to safely perform read and write operations concurrently.

## Public API

The key-value store exposes the following three APIs:

1. **`put(string key, string value)`**
   Inserts a key–value pair into the store. If the key already exists, its value is replaced.

2. **`get(string key)`**
   Returns a boolean indicating whether the key exists, along with an output parameter containing the value if the key is found.

3. **`erase(string key)`**
   Removes the key and its associated value from the store, if the key exists.

## Thread Safety Model

The key-value store uses a coarse-grained locking strategy. A single mutex protects the internal `unordered_map`. All public API methods acquire the mutex before accessing or modifying the map, ensuring that operations are thread-safe and serialized.

## Data Structures

The store uses `std::unordered_map<std::string, std::string>` as the internal storage container, owned and managed by the key-value store class. This choice provides average _O(1)_ lookup, insertion, and deletion performance, which is suitable for an in-memory key-value store.

## Invariants

- All accesses to the internal storage are protected by the mutex.
- Each key exists at most once in the store.
- The store owns all keys and values, and callers receive copies of stored values.

## Ownership and Lifetime

The key-value store owns all keys and values. Memory associated with the store is released when the store object is destroyed.
