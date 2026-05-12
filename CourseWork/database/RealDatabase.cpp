#include "RealDatabase.h"
#include "../visitor/IVisitor.h"
#include <thread>
#include <chrono>
#include <iostream>

std::string RealDatabase::request(const std::string& key) {
    std::cout << "[DB] Пошук ключа \"" << key << "\" у реальній базі даних...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // імітація затримки БД

    std::cout << "[DB] Дані знайдено. Повертаємо результат.\n";
    return "SERVER_RESPONSE_FOR_" + key;
}

void RealDatabase::accept(std::shared_ptr<IVisitor> v) {
    if (v) {

        std::cout << "[DB] RealDatabase прийняв Visitor (порожня обробка).\n";
    }
}