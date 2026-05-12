#include "LRUStrategy.h"
#include "../core/CacheEntry.h"

std::string LRUStrategy::selectVictim(const std::unordered_map<std::string, std::shared_ptr<CacheEntry>>& storage) {
    if (storage.empty()) return "";

    std::string victimKey;
    auto oldest = std::chrono::steady_clock::time_point::max();

    for (auto it = storage.begin(); it != storage.end(); ++it) {
        const std::string& currentKey = it->first;
        auto entry = it->second;
        if (entry->lastAccess < oldest) {
            oldest = entry->lastAccess;
            victimKey = currentKey;
        }
    }
    return victimKey;
}