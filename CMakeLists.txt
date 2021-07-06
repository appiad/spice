cmake_minimum_required(VERSION 3.10)
project(cpp_spice VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Catch2 REQUIRED)
find_package(spdlog REQUIRED)

include(CTest)
include(Catch)

add_subdirectory(src)
add_subdirectory(tests)
