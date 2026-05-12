#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include "../core/IDataService.h"
#include "../core/CacheEntry.h"
#include "../observer/IObserver.h"
#include "../visitor/IVisitor.h"
#include "EvictionStrategy.h" 

class CacheProxy : public IDataService {
private:
    std::unordered_map<std::string, std::shared_ptr<CacheEntry>> storage;
    std::shared_ptr<IDataService> realService;
    std::vector<std::shared_ptr<IObserver>> observers;
    std::shared_ptr<EvictionStrategy> evictionStrategy;

    bool hitStatus = false; 

public:
    CacheProxy(std::shared_ptr<IDataService> service, std::shared_ptr<EvictionStrategy> strategy);

    // Метод для Facade, щоб знати, чи був останній запит успішним у кеші
    bool wasHit() const { return hitStatus; }

    std::string request(const std::string& key) override;

    // Патерн Observer
    void addObserver(std::shared_ptr<IObserver> obs);
    void removeObserver(std::shared_ptr<IObserver> observer);

    // Патерн Visitor
    void accept(std::shared_ptr<IVisitor> visitor);

    // Керування стратегіями та пам'яттю
    void setStrategy(std::shared_ptr<EvictionStrategy> newStrategy) { evictionStrategy = newStrategy; }
    void clearStorage() { storage.clear(); }
    size_t getCacheSize() { return storage.size(); }

    // Патерн Strategy в дії: фізичне видалення об'єкта
    void evict() {
        if (evictionStrategy && !storage.empty()) {
            std::string keyToEvict = evictionStrategy->selectVictim(storage);
            if (!keyToEvict.empty()) {
                std::cout << " [EVICTION] Стратегія обрала для видалення: " << keyToEvict << std::endl;
                storage.erase(keyToEvict);
            }
        }
        else {
            std::cout << " [УВАГА] Кеш порожній або стратегію не задано." << std::endl;
        }
    }
};