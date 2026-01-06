#pragma once

#include <string>

namespace kv_store {

class EvictionPolicy {
   public:
    virtual ~EvictionPolicy() = default;

    virtual void onPut(const std::string& key) = 0;
    virtual void onGet(const std::string& key) = 0;
    virtual void onErase(const std::string& key) = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
};

}  // namespace kv_store