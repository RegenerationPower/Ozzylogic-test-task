cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS random)

add_executable(client src/main.cpp)

target_link_libraries(client Boost::random)
