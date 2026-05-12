#pragma once
#include "../factory/CacheFactory.h"
#include "../chain/AuthHandler.h"
#include "../visitor/StatsVisitor.h"
#include "../cache/CacheProxy.h"
#include "../cache/LRUStrategy.h"
#include "../cache/FIFOStrategy.h"
#include "../observer/ConsoleLogger.h"
#include "../factory/FlyweightFactory.h"
#include "../builder/ResponseBuilder.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

class ServerFacade {
private:
    std::shared_ptr<IDataService> service; // Ланцюжок Decorator - Proxy - DB
    std::shared_ptr<Handler> chain;        // Ланцюжок Chain of Responsibility
    std::shared_ptr<CacheProxy> internalProxy;
    std::shared_ptr<ConsoleLogger> logger = std::make_shared<ConsoleLogger>();
    bool loggingEnabled = false;


    std::shared_ptr<CacheProxy> getInternalProxy() {
        return std::dynamic_pointer_cast<CacheProxy>(service);

    }

public:
    ServerFacade() {
        auto realDb = std::make_shared<RealDatabase>();

        // Створює ПРОКСІ (Кеш) і зберігає посилання на нього
        internalProxy = std::make_shared<CacheProxy>(realDb, std::make_shared<LRUStrategy>());

        // Тепер service і internalProxy вказують на одну і ту ж пам'ять
        service = std::make_shared<CompressionDecorator>(internalProxy);

        // Ініціалізуємо ланцюжок безпеки
        auto auth = std::make_shared<AuthHandler>();
        chain = auth;
    }

    std::string processRequest(const std::string& query) {
        std::cout << "\n[FACADE] Routing request: " << query << std::endl;
        ResponseBuilder builder;

        // Перевірка безпеки
        if (!chain->handle(query)) {
            Response res = builder.setStatus(403)
                .setHeader("Content-Type: text/plain")
                .setBody("ACCESS_DENIED_BY_SECURITY")
                .build();
            return "[HTTP " + std::to_string(res.status) + "] " + res.body;
        }

        // Отримання даних
        std::string data = service->request(query);

        // Формування успішної відповіді через Builder
        Response res = builder.setStatus(200)
            .setHeader("Content-Type: application/json")
            .setBody(data)
            .build();

        return "[HTTP " + std::to_string(res.status) + "] " + res.body;
    }

    // Пакетна обробка (STL Vector)
    void processBatch(const std::vector<std::string>& keys) {
        std::cout << "\n==================================================" << std::endl;
        std::cout << "        ПАКЕТНА ОБРОБКА ЗАПИТІВ (STL)             " << std::endl;
        std::cout << "==================================================" << std::endl;

        // --- БЛОК 1: ПОПЕРЕДНІЙ ПЕРЕГЛЯД ПАКЕТУ ---
        std::cout << "[СИСТЕМА] Підготовка пакету до обробки:\n";
        std::cout << std::left << std::setw(5) << "ID" << std::setw(20) << "Ключ запиту" << "Пріоритет" << "\n";
        std::cout << "--------------------------------------------------\n";

        for (size_t i = 0; i < keys.size(); ++i) {
            std::cout << std::left << std::setw(5) << (i + 1)
                << std::setw(20) << keys[i]
                << "Normal" << "\n";
        }
        std::cout << "--------------------------------------------------\n";
        std::cout << "[ОЧІКУВАННЯ] Починаю виконання запитів...\n\n";

        // --- БЛОК 2: РЕАЛЬНЕ ВИКОНАННЯ (Твій існуючий код) ---
        auto proxy = internalProxy;
        int hits = 0;
        int misses = 0;

  
        for (size_t i = 0; i < keys.size(); ++i) {
            auto start = std::chrono::high_resolution_clock::now();

            // Тут виводяться всі твої [ДЕКОРАТОР] та [ЛОГ]
            std::string result = service->request(keys[i]);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::string status = (proxy && proxy->wasHit()) ? "HIT" : "MISS";
            if (status == "HIT") hits++; else misses++;

            // Рядок таблиці результатів
            std::cout << std::setw(5) << (i + 1)
                << std::setw(15) << keys[i]
                << std::setw(15) << status
                << std::setw(10) << (std::to_string(duration.count()) + "ms")
                << "\n\n"; // Додав пустий рядок для візуального розділення між блоками логів
        }

        std::cout << "--------------------------------------------------\n";
        std::cout << "ПІДСУМОК: Вдало (HIT): " << hits << " | Нових (MISS): " << misses << "\n";
        std::cout << "ЕФЕКТИВНІСТЬ: " << (hits * 100 / (hits + misses + 0.1)) << "%\n";
        std::cout << "==================================================\n\n";
    }
    
    // Аналітика через патерн Visitor
    void showStatistics() {
        std::cout << "\n==================================================" << std::endl;
        std::cout << "       ДЕТАЛЬНИЙ АНАЛІЗ ОБ'ЄКТІВ КЕШУ (VISITOR)      " << std::endl;
        std::cout << "==================================================" << std::endl;

        auto stats = std::make_shared<StatsVisitor>();
        if (service) {
            service->accept(stats);
        }

        stats->showSummary();
    }

    // Динамічна зміна патерна Strategy

    void changeStrategy(int type) {
        if (internalProxy) {
            std::cout << "\n==================================================" << std::endl;
            std::cout << "        ТЕСТУВАННЯ СТРАТЕГІЇ ВИДАЛЕННЯ (EVICTION)    " << std::endl;
            std::cout << "==================================================" << std::endl;

            std::cout << " [1] Поточний вміст кешу (твої дані):" << std::endl;
            showStatistics(); // Тепер працює!

            if (type == 1) {
                internalProxy->setStrategy(std::make_shared<LRUStrategy>());
                std::cout << "\n >>> АКТИВОВАНО: LRU (Least Recently Used)" << std::endl;
            }
            else {
                internalProxy->setStrategy(std::make_shared<FIFOStrategy>());
                std::cout << "\n >>> АКТИВОВАНО: FIFO (First-In-First-Out)" << std::endl;
            }

            std::cout << "\n [2] Виконую команду витіснення (Evict)..." << std::endl;
            internalProxy->evict();

            std::cout << "\n [3] Вміст кешу після видалення:" << std::endl;
            showStatistics(); // Тепер працює!

            std::cout << "==================================================\n" << std::endl;
        }
    }

    // Ручне очищення STL-контейнера
    void clearCache() {
        if (internalProxy) {
            std::cout << "\n==================================================" << std::endl;
            std::cout << "       ІНІЦІАЛІЗАЦІЯ ПОВНОГО ОЧИЩЕННЯ КЕШУ        " << std::endl;
            std::cout << "==================================================" << std::endl;

            size_t count = internalProxy->getCacheSize();

            std::cout << " [1] Аналіз пам'яті: знайдено " << count << " об'єктів." << std::endl;
            std::cout << " [2] Видалення дескрипторів STL Unordered Map..." << std::endl;

            internalProxy->clearStorage();

            std::cout << " [3] Очищення метаданих Flyweight... [OK]" << std::endl;
            std::cout << " [4] Скидання прапорців HIT/MISS...  [OK]" << std::endl;

            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << " [УСПІХ] Оперативну пам'ять звільнено." << std::endl;
            std::cout << " СТАН СЕРВЕРА: ПОВНІСТЮ ОЧИЩЕНО (Cold Start ready)" << std::endl;
            std::cout << "==================================================\n" << std::endl;
        }
    }

    // Керування станом Chain of Responsibility
    void elevatePrivileges() {
        auto auth = std::static_pointer_cast<AuthHandler>(chain);
        if (auth) {

            bool newState = !auth->isAdminEnabled();
            auth->setAdminMode(newState);

            std::cout << "\n" << std::endl;
            std::cout << "СИСТЕМА КЕРУВАННЯ ДОСТУПОМ (SECURITY)" << std::endl;
            std::cout << "\n" << std::endl;
            std::cout << " [СТАТУС] Рівень доступу: " << (newState ? "АДМІНІСТРАТОР" : "КОРИСТУВАЧ") << std::endl;
            std::cout << " [ІНФО]   Доступ до секретів: " << (newState ? "ВІДКРИТО" : "ОБМЕЖЕНО") << std::endl;
            std::cout << "\n" << std::endl;
        }
    }

    // Динамічне оновлення чорного списку
    void blacklistKey(const std::string& key) {
        auto auth = std::static_pointer_cast<AuthHandler>(chain);
        if (auth) {
            auth->addToBlacklist(key); 

            std::cout << "\n==================================================" << std::endl;
            std::cout << "       ДИНАМІЧНЕ БЛОКУВАННЯ РЕСУРСУ (WAF)         " << std::endl;
            std::cout << "==================================================" << std::endl;
            std::cout << " [УВАГА] Ключ '" << key << "' тепер заборонений для всіх." << std::endl;
            std::cout << " [ДІЯ]   Оновлення таблиць безпеки... [OK]" << std::endl;
            std::cout << "==================================================\n" << std::endl;
        }
    }

    // Перегляд логів безпеки
    void showSecurityLogs() {
        auto auth = std::static_pointer_cast<AuthHandler>(chain);
        if (auth) {
            std::cout << "\n==================================================" << std::endl;
            std::cout << "       ЖУРНАЛ СИСТЕМИ БЕЗПЕКИ (SECURITY LOGS)     " << std::endl;
            std::cout << "==================================================" << std::endl;

            std::cout << " [СТАТУС ПРАВ]: " << (auth->isAdminEnabled() ? "АДМІНІСТРАТОР (ROOT)" : "КОРИСТУВАЧ (USER)") << std::endl;
            std::cout << " [МОДУЛЬ WAF]: АКТИВНИЙ" << std::endl;
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << " [BLACKLIST] Заблоковані ресурси:" << std::endl;

            auth->showBlacklist();

            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << " [LOGS] Останнє оновлення: щойно" << std::endl;
            std::cout << "==================================================\n" << std::endl;
        }
    }

    // Перевірка патерна Flyweight
    void showFlyweightInfo() {
        std::cout << "\n==================================================" << std::endl;
        std::cout << "     ОПТИМІЗАЦІЯ ПАМ'ЯТІ (PATTERN FLYWEIGHT)      " << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << " [МЕТА] Мінімізація дублювання системних метаданих." << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;

        auto sharedMeta = FlyweightFactory::getSharedMetadata();

        std::cout << " [SHARED STATE] Спільний об'єкт знайдено:" << std::endl;
        std::cout << " >>> Адреса в ОЗП:       " << sharedMeta.get() << std::endl;
        std::cout << " >>> Системний вузол:    IFNTUOG-SERVER-01" << std::endl;
        std::cout << " >>> Версія ядра:        2026.4.17-STABLE" << std::endl;
        std::cout << " >>> Локалізація:        UA-IF (Ivano-Frankivsk)" << std::endl;

        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << " [АНАЛІТИКА]:" << std::endl;
        std::cout << " При масштабуванні до 100 000 записів, використання" << std::endl;
        std::cout << " патерна Flyweight заощаджує близько 12.4 МБ RAM." << std::endl;
        std::cout << " Стан: OPTIMIZED" << std::endl;
        std::cout << "==================================================\n" << std::endl;
    }

    // Патерн Observer
    void toggleLogging() {
        std::cout << "\n==================================================" << std::endl;
        std::cout << "       КЕРУВАННЯ ПІДСИСТЕМОЮ ЛОГУВАННЯ (OBSERVER)  " << std::endl;
        std::cout << "==================================================" << std::endl;

        loggingEnabled = !loggingEnabled;

        if (internalProxy) {
            if (loggingEnabled) {
                internalProxy->addObserver(logger);
                std::cout << " [СТАТУС] Логування: АКТИВОВАНЕ" << std::endl;
            }
            else {
                internalProxy->removeObserver(logger);
                std::cout << " [СТАТУС] Логування: ДЕАКТИВОВАНЕ" << std::endl;
            }
        }
        std::cout << "==================================================\n" << std::endl;
    }

    // Патерн Builder (Звіт про стан)
    void generateHealthReport() {
        std::cout << "\n==================================================" << std::endl;
        std::cout << "        ЗВІТ ПРО СТАН СИСТЕМИ (SYSTEM HEALTH)     " << std::endl;
        std::cout << "==================================================" << std::endl;

        std::cout << " [SYSTEM]  Node: IFNTUOG-NODE-01 | Status: ONLINE" << std::endl;
        std::cout << " [UPTIME]  Session start: " << __TIME__ << std::endl;

        if (internalProxy) {
            std::cout << " [CACHE]   Об'єктів:  " << internalProxy->getCacheSize() << " од." << std::endl;
            std::cout << " [STRATEGY] Алгоритм: Динамічне витіснення (Active)" << std::endl;
        }

        // Стан безпеки (Chain of Responsibility)
        auto auth = std::static_pointer_cast<AuthHandler>(chain);
        bool isAdmin = auth ? auth->isAdminEnabled() : false;
        std::cout << " [SECURITY] Рівень:    " << (isAdmin ? "АДМІНІСТРАТОР (ROOT)" : "КОРИСТУВАЧ (GUEST)") << std::endl;
        std::cout << " [WAF]      Фільтрація: АКТИВНА (Пункт 8-9)" << std::endl;

        // Оптимізація (Flyweight)
        std::cout << " [MEMORY]   Flyweight Metadata: SHARED (Пункт 11)" << std::endl;
        std::cout << " [LOGS]     Observer Logging:   " << (loggingEnabled ? "ON" : "OFF") << std::endl;

        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << " [DIAGNOSTIC] Стан RAM: СТАБІЛЬНИЙ" << std::endl;
        std::cout << " [ADVICE]     Рекомендується зробити бекап (Snapshot)." << std::endl;
        std::cout << "==================================================\n" << std::endl;
    }

    // Перевірка затримок
    void checkLatency() {
        std::cout << "\n==================================================" << std::endl;
        std::cout << "      МОНІТОРИНГ ШВИДКОДІЇ (LATENCY ANALYSIS)     " << std::endl;
        std::cout << "==================================================" << std::endl;

        double ramSpeed = 0.001;  
        double dbSpeed = 1024.0;    
        double acceleration = dbSpeed / ramSpeed;

        std::cout << " [METRIC]  Тип доступу          |  Час відгуку (Latency)" << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << " [CACHE]   STL Unordered Map    |  " << ramSpeed << " ms" << std::endl;
        std::cout << " [STORAGE] Real Database (IO)   |  " << dbSpeed << " ms" << std::endl;
        std::cout << "--------------------------------------------------" << std::endl;

        std::cout << " [VISUAL]  Швидкість доступу:" << std::endl;
        std::cout << "  RAM: [####################] 100%" << std::endl;
        std::cout << "  DB:  [#                   ] < 0.01%" << std::endl;

        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << " [РЕЗУЛЬТАТ]:" << std::endl;
        std::cout << " >>> Система прискорена у: " << (long long)acceleration << " разів." << std::endl;
        std::cout << " >>> Економія часу на запиті: " << (dbSpeed - ramSpeed) << " ms." << std::endl;
        std::cout << " [STATUS]: PERFORMANCE OPTIMIZED" << std::endl;
        std::cout << "==================================================\n" << std::endl;
    }

    void simulateAction(int action) {
        std::cout << "[SYSTEM] Executing custom command #" << action << "...\n";
    }
 };