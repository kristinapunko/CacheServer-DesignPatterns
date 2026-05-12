#pragma once
#include <string>

struct Response {
    std::string body;
    int status;
    std::string header;
};

class ResponseBuilder {
    Response res;
public:
    ResponseBuilder& setBody(std::string b) { res.body = b; return *this; }
    ResponseBuilder& setStatus(int s) { res.status = s; return *this; }
    ResponseBuilder& setHeader(std::string h) { res.header = h; return *this; }
    Response build() { return res; }
};