#pragma once
#include <memory>
#include <string>

class Handler {
protected:
    std::shared_ptr<Handler> next;
public:
    void setNext(std::shared_ptr<Handler> n) { next = n; }
    virtual bool handle(const std::string& req) {
        if (next) return next->handle(req);
        return true;
    }
    virtual ~Handler() = default;
};