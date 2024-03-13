cmake_minimum_required(VERSION 3.10)


project(remote)



add_executable(${CMAKE_PROJECT_NAME} main.cpp
"../common/CSockinit.h"
"../common/CSockinit.cpp"
"../common/proto.h"
)