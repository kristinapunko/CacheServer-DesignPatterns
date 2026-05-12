#pragma once
#include "../database/RealDatabase.h"
#include "../cache/CacheProxy.h"
#include "../cache/LRUStrategy.h" 
#include "../decorators/CompressionDecorator.h"
#include <memory>

class CacheFactory {
public:
    static std::shared_ptr<IDataService> createFullService() {
        auto db = std::make_shared<RealDatabase>();

        // Створює стратегію
        auto strategy = std::make_shared<LRUStrategy>();

        auto proxy = std::make_shared<CacheProxy>(db, strategy);

        // Огортає декоратором
        return std::make_shared<CompressionDecorator>(proxy);
    }
};