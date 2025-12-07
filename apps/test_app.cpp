#include "app_config.hpp"
#include <cassert>
#include <iostream>

int main() {
    AppConfig config;
    config.load();
    assert(config.getInfo() == "config loaded successfully");
    std::cout << "Test passed!\n";
    return 0;
}
