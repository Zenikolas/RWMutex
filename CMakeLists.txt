cmake_minimum_required(VERSION 3.17)
project(RWMutex)

set(CMAKE_CXX_STANDARD 17)

add_executable(RWMutex main.cpp)
target_link_libraries(RWMutex pthread)