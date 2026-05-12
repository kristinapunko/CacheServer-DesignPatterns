#pragma once
#include "IObserver.h"
#include <iostream>

class ConsoleLogger : public IObserver {
public:
    void onEvent(const std::string& message) override {
        std::cout << "[OBSERVER/LOG]: " << message << std::endl;
    }
};