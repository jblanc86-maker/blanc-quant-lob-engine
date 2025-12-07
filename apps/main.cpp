#include "app_config.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
  AppConfig config;
  config.load(); // In future, use config.load_from_args(argc, argv);
  std::cout << "App started with config: " << config.getInfo() << std::endl;
  return 0;
}
