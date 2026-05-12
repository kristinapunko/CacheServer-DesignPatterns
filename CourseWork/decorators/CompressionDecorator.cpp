#include "CompressionDecorator.h"
#include <algorithm>
#include <iostream>

CompressionDecorator::CompressionDecorator(std::shared_ptr<IDataService> service) : wrappee(service) {}

std::string CompressionDecorator::compress(const std::string& data) {
    std::string res = data;
    std::reverse(res.begin(), res.end()); 
    return res;
}

std::string CompressionDecorator::decompress(const std::string& data) {
    std::string res = data;
    std::reverse(res.begin(), res.end()); 
    return res;
}

std::string CompressionDecorator::request(const std::string& key) {
    std::cout << "\n[ÄÅÊÎÐÀÒÎÐ] ²í³ö³àë³çàö³ÿ çàõèùåíîãî ïðîòîêîëó..." << std::endl;

    std::string data = wrappee->request(key);

    std::cout << "[ÄÅÊÎÐÀÒÎÐ] Îòðèìàíî ñèð³ äàí³. Ðîçì³ð: " << data.length() << " áàéò." << std::endl;
    std::cout << "[ÄÅÊÎÐÀÒÎÐ] Çàñòîñóâàííÿ àëãîðèòìó øèôðóâàííÿ (Simulation)..." << std::endl;

    std::string encrypted = "[ÇÀÕÈÙÅÍ²_ÄÀÍ²: " + data + "]";

    std::cout << "[ÄÅÊÎÐÀÒÎÐ] Øèôðóâàííÿ çàâåðøåíî. Ïåðåäà÷à â³äïîâ³ä³ ôàñàäó." << std::endl;
    return encrypted;
}

void CompressionDecorator::accept(std::shared_ptr<IVisitor> v) {
    wrappee->accept(v);
}