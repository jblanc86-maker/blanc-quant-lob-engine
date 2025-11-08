cmake_minimum_required(VERSION 3.16)
project(itch_lob_proto CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add executables tied to source files
add_executable(gen_synth src/gen_synth.cpp)
add_executable(replay src/replay.cpp)

# Optional: Boost for CLI args
find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(gen_synth PRIVATE Boost::program_options)
target_link_libraries(replay PRIVATE Boost::program_options)

# Enable optimization + warnings
target_compile_options(gen_synth PRIVATE -O3 -Wall -Wextra)
target_compile_options(replay    PRIVATE -O3 -Wall -Wextra)
