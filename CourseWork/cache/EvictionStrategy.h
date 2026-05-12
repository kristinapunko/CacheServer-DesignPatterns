#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "../core/CacheEntry.h"

class EvictionStrategy {
public:
    virtual ~EvictionStrategy() = default;
    virtual std::string selectVictim(const std::unordered_map<std::string,
        std::shared_ptr<CacheEntry>>&storage) = 0;
};