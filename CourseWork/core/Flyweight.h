#pragma once
#include <string>
#include <memory>

struct ServerMetadata {
    std::string serverName = "IFNTUOG-Cloud-Server";
    std::string version = "v3.2.1-stable";
};

class FlyweightFactory {
public:
    static std::shared_ptr<ServerMetadata> getSharedMetadata() {
        static std::shared_ptr<ServerMetadata> instance = std::make_shared<ServerMetadata>();
        return instance;
    }
};