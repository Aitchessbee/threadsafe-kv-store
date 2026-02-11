#pragma once

#include <string>
#include <vector>

#include "snapshot_entry.h"

namespace kv_store {

class SnapshotManager {
   public:
    explicit SnapshotManager(const std::string& path);

    void saveSnapshot(const std::vector<SnapshotEntry>& entries);

    std::vector<SnapshotEntry> loadSnapshot();

   private:
    std::string path_;
};

}  // namespace kv_store