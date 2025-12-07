#pragma once
#include <string>

class AppConfig
{
public:
  void load();
  void load_from_args(int argc, char *argv[]); // Placeholder for argument-based config
  std::string getInfo() const;
  std::string summary() const { return getInfo(); }

private:
  std::string info_ = "default config";
};
