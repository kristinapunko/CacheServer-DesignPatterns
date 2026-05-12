#pragma once
#include "../core/IDataService.h"
#include <string>

class RealDatabase : public IDataService {
public:
    std::string request(const std::string& key) override;
    void accept(std::shared_ptr<IVisitor> v) override;
};
