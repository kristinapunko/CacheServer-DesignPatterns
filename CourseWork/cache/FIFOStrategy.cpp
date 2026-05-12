#include "FIFOStrategy.h"
#include "../core/CacheEntry.h"

std::string FIFOStrategy::selectVictim(const std::unordered_map<std::string, std::shared_ptr<CacheEntry>>& storage) {
    while (!insertionOrder.empty()) {
        std::string candidate = insertionOrder.front();
        insertionOrder.pop(); 

        if (storage.find(candidate) != storage.end()) {
            return candidate;
        }
    }
    return ""; 
}

void FIFOStrategy::recordInsertion(const std::string& key) {
    insertionOrder.push(key);
}