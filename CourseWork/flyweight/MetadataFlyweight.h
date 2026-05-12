#pragma once
#include <string>
#include <unordered_map>
#include <memory>

class Metadata {
public:
    std::string serverName = "HighSpeed-Server-v1.0";
    std::string region = "Europe-West";
};

class FlyweightFactory {
    static std::shared_ptr<Metadata> shared;
public:
    static std::shared_ptr<Metadata> getMetadata() {
        if (!shared) shared = std::make_shared<Metadata>();
        return shared;
    }
};