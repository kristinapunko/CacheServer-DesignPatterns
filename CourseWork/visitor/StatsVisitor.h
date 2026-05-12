#pragma once
#include "IVisitor.h"
#include <iostream>
#include <string>
#include <iomanip> 
#include "../core/CacheEntry.h"

class StatsVisitor : public IVisitor {
private:
    int count = 0;
    size_t totalSize = 0;
    int totalObjects = 0;
    size_t totalBytes = 0;
    int totalHits = 0;

public:

    // Метод, який викликається для кожного елемента кешу
    void visit(const std::string& key, const std::shared_ptr<CacheEntry>& entry) override {
        totalObjects++;
        totalHits += entry->hits;
        totalBytes += entry->data.size(); 

        std::cout << " [СКАНУВАННЯ] Ключ: " << std::left << std::setw(12) << key
            << " | Hits: " << entry->hits << " | Розмір: " << entry->data.size() << " байт" << std::endl;
    }

    void showSummary() {
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << "        ПІДСУМКОВИЙ ЗВІТ СКАНУВАННЯ ПАМ'ЯТІ        " << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << " > Всього об'єктів у кеші:    " << totalObjects << " од." << std::endl;
        std::cout << " > Загальний обсяг даних:     " << totalBytes << " байт" << std::endl;
        std::cout << " > Сумарна кількість запитів: " << totalHits << std::endl;

        if (totalObjects > 0) {
            std::cout << " > Середня популярність:      " << (float)totalHits / totalObjects << std::endl;
        }
        std::cout << "--------------------------------------------------" << std::endl;
    }
};