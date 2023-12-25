cmake_minimum_required(VERSION 3.5)

project(CathClient)

# 启用自动处理UI文件和MOC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置使用的Qt版本，如Qt5
find_package(Qt5 COMPONENTS Widgets webenginewidgets REQUIRED)


# 包含的文件
set(PROJECT_SOURCES
        chatclient.cpp
        chatclient.h
        chatclient.ui
        "../common/Proto.h"
        "../common/CSockinit.h"
        "../common/CSockinit.cpp"
        main.cpp
)


add_executable(CathClient ${PROJECT_SOURCES})

target_link_libraries(CathClient PRIVATE Qt5::Widgets Qt5::WebEngineWidgets)
