#include "kv_store/persistence/snapshot_manager.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <stdexcept>

namespace kv_store {

SnapshotManager::SnapshotManager(const std::string& path) : path_(path) {
}

void SnapshotManager::saveSnapshot(const std::vector<SnapshotEntry>& entries) {
    std::ofstream out(path_, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Failed to open snapshot file for writing");
    }

    uint64_t count = entries.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& entry : entries) {
        uint64_t keySize = entry.key.size();
        out.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
        out.write(entry.key.data(), keySize);

        uint64_t valueSize = entry.value.size();
        out.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
        out.write(entry.value.data(), valueSize);

        auto expireSeconds = std::chrono::duration_cast<std::chrono::seconds>(entry.expires_at.time_since_epoch()).count();

        int64_t expire = static_cast<int64_t>(expireSeconds);
        out.write(reinterpret_cast<const char*>(&expire), sizeof(expire));
    }

    if (!out.good()) {
        throw std::runtime_error("Failed while writing snapshot file");
    }

    out.flush();
}

std::vector<SnapshotEntry> SnapshotManager::loadSnapshot() {
    std::ifstream in(path_, std::ios::binary);
    if (!in) {
        return {};
    }

    uint64_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    if (!in.good()) {
        throw std::runtime_error("Failed to read snapshot header");
    }

    std::vector<SnapshotEntry> entries;
    entries.reserve(count);

    for (uint64_t i = 0; i < count; ++i) {
        SnapshotEntry entry;

        uint64_t keySize = 0;
        in.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        entry.key.resize(keySize);
        in.read(entry.key.data(), keySize);

        uint64_t valueSize = 0;
        in.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        entry.value.resize(valueSize);
        in.read(entry.value.data(), valueSize);

        int64_t expireSeconds = 0;
        in.read(reinterpret_cast<char*>(&expireSeconds), sizeof(expireSeconds));
        entry.expires_at = std::chrono::system_clock::time_point{std::chrono::seconds(expireSeconds)};

        if (!in.good()) {
            throw std::runtime_error("Corrupted snapshot file");
        }

        entries.push_back(std::move(entry));
    }

    return entries;
}

}  // namespace kv_store
