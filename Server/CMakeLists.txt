cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS random json)

add_executable(server src/main.cpp)

target_link_libraries(server Boost::random Boost::json)
