#pragma once
#include "../Core/IDataService.h"
#include <memory>

class CompressionDecorator : public IDataService {
private:
    std::shared_ptr<IDataService> wrappee;
    std::string compress(const std::string& data);
    std::string decompress(const std::string& data);

public:
    CompressionDecorator(std::shared_ptr<IDataService> service);
    std::string request(const std::string& key) override;

    void accept(std::shared_ptr<IVisitor> v) override;
};

