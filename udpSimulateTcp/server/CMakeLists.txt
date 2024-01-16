cmake_minimum_required(VERSION 3.10)
project(udpSimulateTcpServer)
add_executable(${PROJECT_NAME} 
main.cpp
"../common/CSockinit.h"
"../common/CSockinit.cpp"
"../common/CLock.h"
"../common/CLock.cpp"
"../common/CCrc.h"
"../common/CCrc.cpp"
"../common/CByteStreamBuff.h"
"../common/CByteStreamBuff.cpp"
"../common/CUMT.h"
"../common/CUMT.cpp"
)

