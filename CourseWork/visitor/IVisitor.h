#pragma once
#include <string>
#include <memory>
#include "../core/CacheEntry.h" 

class IVisitor {
public:
    virtual ~IVisitor() = default;

    virtual void visit(const std::string& key, const std::shared_ptr<CacheEntry>& entry) = 0;
};