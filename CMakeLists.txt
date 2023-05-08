cmake_minimum_required(VERSION 3.0.0)
project(corazones VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_PREFIX_PATH "C:\\dev\\vcpkg\\packages\\nlohmann-json_x86-windows")

add_executable(corazones main.cpp CSVData.cpp)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(corazones nlohmann_json::nlohmann_json)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
