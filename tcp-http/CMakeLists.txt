cmake_minimum_required(VERSION 3.20)
project(tcp_http)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_http main.cpp CSockinit.h CSockinit.cpp )
