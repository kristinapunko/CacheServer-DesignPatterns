//#pragma once
//#include "EvictionStrategy.h"
#pragma once
#include "EvictionStrategy.h"
#include <queue>
#include <unordered_set>

class FIFOStrategy : public EvictionStrategy {
private:
    std::queue<std::string> insertionOrder;
    std::unordered_set<std::string> currentKeys;  

public:
    std::string selectVictim(const std::unordered_map<std::string, 
                              std::shared_ptr<CacheEntry>>& storage) override;

    void recordInsertion(const std::string& key);  
};