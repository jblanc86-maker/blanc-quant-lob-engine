cmake_minimum_required(VERSION 3.16)
project(app_main)

add_executable(app_main main.cpp app_config.cpp)
add_executable(test_app test_app.cpp app_config.cpp)
add_executable(bench_app bench_app.cpp app_config.cpp)

# Optionally link libraries here
# target_link_libraries(app_main ...)
