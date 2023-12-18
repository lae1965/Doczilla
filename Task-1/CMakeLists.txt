cmake_minimum_required(VERSION 3.20)
project(txtparser)

add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
