#pragma once
#include <string>
#include <memory>
#include "../visitor/IVisitor.h" // Потрібен інклюд візитора

class IDataService {
public:
    virtual std::string request(const std::string& key) = 0;
    virtual void accept(std::shared_ptr<IVisitor> v) = 0;
    virtual ~IDataService() = default;
};