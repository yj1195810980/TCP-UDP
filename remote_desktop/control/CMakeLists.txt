cmake_minimum_required(VERSION 3.10)


project(control)


# 启用自动处理UI文件和MOC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置使用的Qt版本，如Qt5
find_package(Qt5 COMPONENTS Widgets webenginewidgets REQUIRED)



add_executable(${CMAKE_PROJECT_NAME} main.cpp
mainwindow.cpp
mainwindow.h
mainwindow.ui
"../common/CSockinit.h"
"../common/CSockinit.cpp"
"../common/proto.h"
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Widgets)