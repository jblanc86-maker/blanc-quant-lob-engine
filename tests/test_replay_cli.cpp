// SPDX-License-Identifier: Apache-2.0
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

namespace
{
  struct CommandResult
  {
    int exit_code{};
    std::string output;
  };

  CommandResult run_command(const std::vector<std::string> &args)
  {
    int fd[2];
    if (pipe(fd) != 0)
    {
      return {-1, std::string("pipe failed: ") + std::strerror(errno)};
    }

    pid_t pid = fork();
    if (pid < 0)
    {
      close(fd[0]);
      close(fd[1]);
      return {-1, std::string("fork failed: ") + std::strerror(errno)};
    }

    if (pid == 0)
    {
      close(fd[0]);
      (void)dup2(fd[1], STDOUT_FILENO);
      (void)dup2(fd[1], STDERR_FILENO);
      close(fd[1]);

      std::vector<std::vector<char>> argv_storage;
      argv_storage.reserve(args.size() + 1);
      argv_storage.emplace_back(std::strlen(REPLAY_BIN_PATH) + 1);
      std::memcpy(argv_storage.back().data(), REPLAY_BIN_PATH, argv_storage.back().size());
      for (const auto &arg : args)
      {
        argv_storage.emplace_back(arg.size() + 1);
        std::memcpy(argv_storage.back().data(), arg.c_str(), argv_storage.back().size());
      }

      std::vector<char *> argv;
      argv.reserve(args.size() + 2);
      for (auto &value : argv_storage)
      {
        argv.push_back(value.data());
      }
      argv.push_back(nullptr);
      execv(REPLAY_BIN_PATH, argv.data());

      std::cerr << "execv failed: " << std::strerror(errno) << std::endl;
      _exit(127);
    }

    close(fd[1]);
    std::string output;
    char buffer[256];
    ssize_t n = 0;
    while ((n = read(fd[0], buffer, sizeof(buffer))) > 0)
    {
      output.append(buffer, static_cast<size_t>(n));
    }
    close(fd[0]);

    int rc = 0;
    if (waitpid(pid, &rc, 0) < 0)
    {
      return {-1, std::string("waitpid failed: ") + std::strerror(errno)};
    }
    if (WIFEXITED(rc))
    {
      rc = WEXITSTATUS(rc);
    }
    else if (WIFSIGNALED(rc))
    {
      rc = 128 + WTERMSIG(rc);
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
