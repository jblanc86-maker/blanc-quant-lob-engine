// SPDX-License-Identifier: Apache-2.0
#include <array>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <sys/wait.h>

namespace
{
  struct CommandResult
  {
    int exit_code{};
    std::string output;
  };

  std::string shell_quote(const std::string &value)
  {
    std::string quoted = "'";
    for (char c : value)
    {
      if (c == '\'')
      {
        quoted += "'\\''";
      }
      else
      {
        quoted += c;
      }
    }
    quoted += "'";
    return quoted;
  }

  CommandResult run_command(const std::vector<std::string> &args)
  {
    std::string command = shell_quote(REPLAY_BIN_PATH);
    for (const auto &arg : args)
    {
      command += " ";
      command += shell_quote(arg);
    }
    command += " 2>&1";

    std::array<char, 256> buffer{};
    std::string output;
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
      return {-1, "popen failed"};
    }

    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
      output += buffer.data();
    }

    int rc = pclose(pipe);
    if (WIFEXITED(rc))
    {
      rc = WEXITSTATUS(rc);
    }

    return {rc, output};
  }

  bool expect_failure(const std::vector<std::string> &args,
                      int expected_exit_code,
                      const std::string &expected_output)
  {
    const CommandResult result = run_command(args);
    if (result.exit_code != expected_exit_code)
    {
      std::cerr << "unexpected exit code " << result.exit_code
                << " (expected " << expected_exit_code << ")\n"
                << result.output << std::endl;
      return false;
    }
    if (result.output.find(expected_output) == std::string::npos)
    {
      std::cerr << "missing output: " << expected_output << "\n"
                << result.output << std::endl;
      return false;
    }
    return true;
  }
} // namespace

int main()
{
  if (!std::filesystem::exists(REPLAY_BIN_PATH))
  {
    std::cerr << "missing replay binary: " << REPLAY_BIN_PATH << std::endl;
    return 10;
  }

  if (!expect_failure({"--gap-ppm", "nan"}, 1, "Invalid value for --gap-ppm: nan"))
  {
    return 1;
  }
  if (!expect_failure({"--gap-ppm", "-1"}, 1, "Invalid value for --gap-ppm: -1"))
  {
    return 2;
  }
#ifdef __linux__
  if (!expect_failure({"--cpu-pin", "999999"}, 1, "Invalid --cpu-pin:"))
  {
    return 3;
  }
#endif

  std::cout << "replay cli validation passed" << std::endl;
  return 0;
}
