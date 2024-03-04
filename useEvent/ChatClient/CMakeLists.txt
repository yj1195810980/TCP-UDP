cmake_minimum_required(VERSION 3.10)

project(ChatClient)

add_executable(${PROJECT_NAME} 
main.cpp
"../common/CSockinit.h"
"../common/CSockinit.cpp"
)

