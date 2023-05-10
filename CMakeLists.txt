cmake_minimum_required(VERSION 3.0.0)

project("corazones")

set(CMAKE_CXX_STANDARD 11)

add_executable(corazones main.cpp CSVData.cpp)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(corazones PRIVATE nlohmann_json::nlohmann_json)