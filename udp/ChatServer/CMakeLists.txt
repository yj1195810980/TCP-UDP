cmake_minimum_required(VERSION 3.10)


project(ChatServer)



add_executable(${PROJECT_NAME} 
ChatServer.cpp
CLock.cpp
CLock.h
"../common/Proto.h"
"../common/CSockinit.h"
"../common/CSockinit.cpp"
)

