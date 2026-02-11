#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "kv_store/kv_store.h"

using namespace kv_store;

void print_help() {
    std::cout << "\nAvailable Commands:\n"
              << "  put <key> <val> [ttl] - Insert/Update\n"
              << "  get <key>             - Retrieve value\n"
              << "  erase <key>           - Remove key\n"
              << "  save                  - Save snapshot\n"
              << "  load                  - Load snapshot\n"
              << "  help/exit\n";
}

int main() {
    KVStoreOptions options;
    options.num_shards = 4;
    options.snapshot_path = "store.bin";
    KVStore store(options);

    std::string line;
    std::cout << "KVStore CLI (v1.0)\nType 'help' for commands.\n";

    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string cmd, key, val;
        ss >> cmd;

        if (cmd == "exit")
            break;
        if (cmd == "help") {
            print_help();
            continue;
        }

        if (cmd == "put") {
            int ttl = 0;
            ss >> key >> val;
            if (ss >> ttl) {
                store.put(key, val, std::chrono::seconds(ttl));
            } else {
                store.put(key, val, std::chrono::seconds(0));
            }
            std::cout << "OK\n";
        } else if (cmd == "get") {
            ss >> key;
            auto [found, result] = store.get(key);
            if (found)
                std::cout << "\"" << result << "\"\n";
            else
                std::cout << "(nil)\n";
        } else if (cmd == "erase") {
            ss >> key;
            store.erase(key);
            std::cout << "OK\n";
        } else if (cmd == "save") {
            store.saveSnapshot();
            std::cout << "Snapshot saved to " << options.snapshot_path << "\n";
        } else if (cmd == "load") {
            if (std::filesystem::exists(options.snapshot_path)) {
                store.loadSnapshot();
                std::cout << "Snapshot loaded.\n";
            } else {
                std::cout << "Error: Snapshot file not found (" << options.snapshot_path << ")\n";
            }
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }
    return 0;
}