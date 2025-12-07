#pragma once
#include <string>

class AppConfig
{
public:
  void load();
  std::string getInfo() const;

private:
  std::string info_ = "default config";
};
