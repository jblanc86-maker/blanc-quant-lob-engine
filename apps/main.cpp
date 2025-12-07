#include "app_config.hpp"
#include <iostream>

int main()
{
  std::cout << "App started.\n";
  // Example usage of config
  AppConfig config;
  config.load();
  std::cout << "Config loaded: " << config.getInfo() << std::endl;
  return 0;
}
