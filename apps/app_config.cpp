#include "app_config.hpp"
#include <iostream>

void AppConfig::load()
{
  // Simulate loading config
  info_ = "config loaded successfully";
}

std::string AppConfig::getInfo() const
{
  return info_;
}
