// SPDX-License-Identifier: MIT
#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef MAX_REPLAY_BYTES
#define MAX_REPLAY_BYTES (256ULL * 1024 * 1024)
#endif

inline std::vector<char> read_all_bounded(const std::string &path) {
  namespace fs = std::filesystem;
  if (!fs::exists(path))
    throw std::runtime_error("file not found: " + path);
  auto sz = fs::file_size(path);
  if (sz > MAX_REPLAY_BYTES)
    throw std::runtime_error("file too large (guard): " + std::to_string(sz));
  std::vector<char> buf(static_cast<size_t>(sz));
  std::ifstream ifs(path, std::ios::binary);
  if (!ifs)
    throw std::runtime_error("open failed: " + path);
  if (!ifs.read(buf.data(), static_cast<std::streamsize>(sz)))
    throw std::runtime_error("read failed");
  return buf;
}
