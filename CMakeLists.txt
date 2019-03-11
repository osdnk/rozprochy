cmake_minimum_required(VERSION 3.9)
project(token_ring)

set(CMAKE_CXX_STANDARD 11)

add_executable(main main.cpp TCP_client.cpp TokenRingUtils.hpp)

target_link_libraries(main WS2_32.lib)


