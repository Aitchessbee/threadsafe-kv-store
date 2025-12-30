#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <utility>

namespace kv_store {
    
class KVStore {
public:
    std::pair<bool, std::string> get(const std::string& key) const;
    void put(const std::string& key, const std::string& value);
    void erase(const std::string& key);

private:
    std::unordered_map<std::string, std::string> store_;
    mutable std::mutex mutex_;
};

}
