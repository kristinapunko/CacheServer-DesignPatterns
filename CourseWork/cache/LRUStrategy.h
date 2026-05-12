//#pragma once
//#include "EvictionStrategy.h"
//#include <iostream>
//
//class LRUStrategy : public EvictionStrategy {
#pragma once
#include "EvictionStrategy.h"
#include <chrono>

class LRUStrategy : public EvictionStrategy {
public:
    std::string selectVictim(const std::unordered_map<std::string,
        std::shared_ptr<CacheEntry>>&storage) override;
};