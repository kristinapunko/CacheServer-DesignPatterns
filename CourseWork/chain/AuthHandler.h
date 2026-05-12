#pragma once
#include "Handler.h"
#include <iostream>
#include <vector>
#include <algorithm>

class AuthHandler : public Handler {
private:
    std::vector<std::string> blacklist;
    bool isAdmin = false;

public:
    AuthHandler() {
        // Початковий чорний список
        blacklist.push_back("virus_exe");
        blacklist.push_back("malware_link");
    }

    bool isAdminEnabled() const {
        return isAdmin;
    }

    bool handle(const std::string& req) override {

        // Якщо ключ у списку — вхід закритий
        if (std::find(blacklist.begin(), blacklist.end(), req) != blacklist.end()) {
            std::cout << "[SECURITY] КЛЮЧ ЗАБЛОКОВАНО: '" << req << "' у чорному списку!\n";
            return false;
        }

        // Додаємо сюди admin_data, щоб його не бачили звичайні юзери
        if ((req == "admin_panel" || req == "admin_data") && !isAdmin) {
            std::cout << "[AUTH] ВІДМОВА: Потрібні права адміністратора для '" << req << "'.\n";
            return false;
        }

        std::cout << "[AUTH] Авторизація успішна для: " << req << "\n";
        return Handler::handle(req);
    }

    void addToBlacklist(const std::string& key) {
        blacklist.push_back(key);
        std::cout << "[SECURITY] Key '" << key << "' added to blacklist.\n";
    }

    void setAdminMode(bool status) {
        isAdmin = status;
        std::cout << "[AUTH] Admin mode " << (isAdmin ? "ENABLED" : "DISABLED") << ".\n";
    }

    void showBlacklist() {
        std::cout << "--- CURRENT BLACKLIST ---\n";
        for (const auto& item : blacklist) {
            std::cout << " - " << item << "\n";
        }
    }
};