cmake_minimum_required(VERSION 3.27)
project(ClionThreadPool)

set(CMAKE_CXX_STANDARD 17)

add_executable(ClionThreadPool main.cpp
        CThreadPool.cpp
        CThreadPool.h
        CLock.h
        CLock.cpp
)
