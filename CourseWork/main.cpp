#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <locale>
#include "facade/ServerFacade.h"
#include <windows.h>

void showMenu() {
    std::cout << "\n СЕРВЕР КЕШ-КОНТРОЛЬ \n";
    std::cout << "1. Звичайний запит        2. Захищений запит\n";
    std::cout << "3. Пакетна обробка       4. LRU стратегія\n";
    std::cout << "5. FIFO стратегія        6. Очистити кеш\n";
    std::cout << "7. Підвищити доступ      8. Заблокувати ключ\n";
    std::cout << "9. Логи безпеки          10. Аналіз кешу\n";
    std::cout << "11. Flyweight            12. Логування\n";
    std::cout << "13. Стан системи         14. Затримка\n";
    std::cout << "15. ВИХІД\n";
    std::cout << "\n";
    std::cout << "Ваш вибір: ";
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    ServerFacade server;
    int choice;
    std::string key;

    while (true) {
        showMenu();
        if (!(std::cin >> choice)) break;

        switch (choice) {
        case 1:
        case 2:
            std::cout << "Введіть ключ: ";
            std::cin >> key;
            std::cout << "Відповідь: " << server.processRequest(key) << "\n";
            break;

        case 3: {
            std::vector<std::string> batch;

            std::cout << "\n[ПАКЕТНИЙ РЕЖИМ] Введіть ключі через пробіл:\n> ";

            std::string line;
            std::getline(std::cin >> std::ws, line); // читаємо весь рядок

            std::stringstream ss(line);
            std::string key;

            while (ss >> key) {
                // очищаємо від пунктуації (коми, крапки і т.д.)
                key.erase(std::remove_if(key.begin(), key.end(),
                    [](unsigned char c) {
                        return std::ispunct(c);
                    }), key.end());

                if (!key.empty()) {
                    batch.push_back(key);
                }
            }

            if (!batch.empty()) {
                server.processBatch(batch);
            }
            else {
                std::cout << ">>> Помилка: Список ключів порожній!\n";
            }

            break;
        }

        case 4: server.changeStrategy(1); break;
        case 5: server.changeStrategy(2); break;
        case 6: server.clearCache(); break;

        case 7: server.elevatePrivileges(); break;

        case 8:
            std::cout << "Введіть ключ для блокування: ";
            std::cin >> key;
            server.blacklistKey(key);
            break;

        case 9: server.showSecurityLogs(); break;
        case 10: server.showStatistics(); break;
        case 11: server.showFlyweightInfo(); break;
        case 12: server.toggleLogging(); break;
        case 13: server.generateHealthReport(); break;
        case 14: server.checkLatency(); break;
        case 15: return 0;

        default: server.simulateAction(choice); break;
        }
    }

    return 0;
}