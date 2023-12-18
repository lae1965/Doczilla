cmake_minimum_required(VERSION 3.20)
project(students)

find_package(PkgConfig REQUIRED)
find_package(SQLite3 REQUIRED)
pkg_check_modules(JSON REQUIRED IMPORTED_TARGET nlohmann_json)

add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} SQLite::SQLite3 PkgConfig::JSON)
