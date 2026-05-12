#pragma once
#include <string>
#include <chrono>
#include <memory>

class ServerMetadata;  

struct CacheEntry {
    std::string data;
    int hits = 0;
    std::chrono::steady_clock::time_point lastAccess;

    std::shared_ptr<ServerMetadata> metadata;

    CacheEntry(const std::string& d, std::shared_ptr<ServerMetadata> meta)
        : data(d),
        hits(0),
        lastAccess(std::chrono::steady_clock::now()),
        metadata(meta)
    {
    }
};