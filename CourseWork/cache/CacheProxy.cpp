#include "CacheProxy.h"
#include "../core/CacheEntry.h"
#include "../visitor/IVisitor.h"
#include "../factory/FlyweightFactory.h"
#include "../cache/FIFOStrategy.h"              
#include <iostream>
#include <algorithm> 
#include <chrono>

CacheProxy::CacheProxy(std::shared_ptr<IDataService> service, std::shared_ptr<EvictionStrategy> strategy)
    : realService(service), evictionStrategy(strategy), hitStatus(false) {
}

std::string CacheProxy::request(const std::string& key) {
    hitStatus = false; 
    std::cout << "\n[ЛОГ] [ЗАПИТ] Перевірка кешу для ключа: " << key << "..." << std::endl;

    auto it = storage.find(key);
    if (it != storage.end()) {
        hitStatus = true;
        std::cout << ">>> [ПОЗИТИВНИЙ ВІДГУК КЕШУ] (RAM HIT)" << std::endl;

        // Оновлюємо статистику для LFU та час доступу для LRU
        it->second->hits++;
        it->second->lastAccess = std::chrono::steady_clock::now();

        // Сповіщаємо спостерігачів (Observer)
        for (auto& obs : observers) {
            if (obs) obs->onEvent("Cache HIT for key: " + key);
        }

        return it->second->data;
    }

    // Якщо даних немає — йдемо в "реальну БД"
    std::cout << ">>> [ПРОМАХ КЕШУ] Звернення до бази даних..." << std::endl;
    std::string result = realService->request(key);

    // Патерн Flyweight: отримуємо спільний об'єкт метаданих
    auto sharedMeta = FlyweightFactory::getSharedMetadata();

    // Створюємо новий запис (використовуємо конструктор з 2 параметрами)
    auto newEntry = std::make_shared<CacheEntry>(result, sharedMeta);
    storage[key] = newEntry;

    // Якщо активована стратегія FIFO — додаємо ключ у чергу вставки
    if (auto fifo = std::dynamic_pointer_cast<FIFOStrategy>(evictionStrategy)) {
        fifo->recordInsertion(key);
    }

    for (auto& obs : observers) {
        if (obs) obs->onEvent("Cache MISS for key: " + key + ". Data fetched from DB.");
    }

    return result;
}

void CacheProxy::addObserver(std::shared_ptr<IObserver> obs) {
    if (obs) observers.push_back(obs);
}

void CacheProxy::removeObserver(std::shared_ptr<IObserver> observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void CacheProxy::accept(std::shared_ptr<IVisitor> visitor) {
    if (!visitor) return;
    for (auto const& [key, entry] : storage) {
        visitor->visit(key, entry);
    }
}