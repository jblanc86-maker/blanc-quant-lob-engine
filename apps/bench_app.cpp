#include "app_config.hpp"
#include <chrono>
#include <iostream>

int main() {
    AppConfig config;
    auto start = std::chrono::high_resolution_clock::now();
    config.load();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Config load benchmark: " << elapsed.count() << " ms\n";
    return 0;
}
