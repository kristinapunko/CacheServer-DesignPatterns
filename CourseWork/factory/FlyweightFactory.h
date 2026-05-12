#pragma once
#include <memory>
#include <string>

class ServerMetadata {
public:
    std::string serverName = "HighSpeed-Server-v1.0";
    std::string region = "Europe-West";
};

class FlyweightFactory {
private:
    static std::shared_ptr<ServerMetadata> shared;
public:
    static std::shared_ptr<ServerMetadata> getSharedMetadata() {
        if (!shared) shared = std::make_shared<ServerMetadata>();
        return shared;
    }
};