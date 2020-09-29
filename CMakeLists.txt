cmake_minimum_required(VERSION 3.0.0)
project(si5351_config_seeq VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(src)
add_executable(Main test/main.cpp )
target_include_directories(Main PUBLIC inc)
target_include_directories(Si5351configseek PUBLIC inc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(Main Si5351configseek)