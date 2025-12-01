// SPDX-License-Identifier: Apache-2.0
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>

int main()
{
  const char *BIN = "./bin/replay";
  const std::string INPUT = "../data/golden/itch_1m.bin";
  const std::string DIGEST_FILE = "../data/golden/itch_1m.fnv";

  // Run the replay binary and capture output
  std::string cmd = std::string(BIN) + " --input " + INPUT + " 2>&1";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
  {
    std::cerr << "Failed to run replay binary" << std::endl;
    return 2;
  }
  char buffer[256];
  std::string output;
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
  {
    output += buffer;
  }
  int rc = pclose(pipe);
  if (rc != 0)
  {
    std::cerr << "replay failed with rc=" << rc << std::endl;
    return 3;
  }

  // Extract digest from output
  std::string marker = "digest_fnv=0x";
  auto pos = output.find(marker);
  if (pos == std::string::npos)
  {
    std::cerr << "digest marker not found in replay output" << std::endl;
    std::cerr << output << std::endl;
    return 4;
  }
  pos += marker.size();
  auto end = output.find_first_not_of("0123456789abcdef", pos);
  std::string digest = output.substr(pos, end - pos);

  // Read golden digest
  std::ifstream f(DIGEST_FILE);
  if (!f)
  {
    std::cerr << "Could not open " << DIGEST_FILE << std::endl;
    return 5;
  }
  std::string golden;
  std::getline(f, golden);
  if (golden.empty())
  {
    std::cerr << "empty golden digest file" << std::endl;
    return 6;
  }

  if (digest != golden)
  {
    std::cerr << "Golden mismatch: got " << digest << " expected " << golden << std::endl;
    return 7;
  }

  std::cout << "Golden digest match: " << digest << std::endl;
  return 0;
}
