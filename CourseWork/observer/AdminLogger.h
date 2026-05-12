#pragma once
#include "IObserver.h"
#include <iostream>

class AdminLogger : public IObserver {
public:
    void onEvent(const std::string& message) override {
        std::cout << "[NOTIFY ADMIN]: " << message << std::endl;
    }
};