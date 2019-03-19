cmake_minimum_required(VERSION 3.9)
project(token_ring)

set(CMAKE_CXX_STANDARD 11)

add_executable(main main.cpp TCP_client.cpp TCP_client.hpp TokenRingUtils.hpp TokenRingUtils.cpp Token.cpp Token.hpp)



