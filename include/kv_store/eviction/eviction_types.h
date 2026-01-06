#pragma once

namespace kv_store {

enum class EvictionType {
    None,
    LRU,
    TTL,
};

}  // namespace kv_store