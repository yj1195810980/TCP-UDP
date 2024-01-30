cmake_minimum_required(VERSION 3.10)


project(myIcmp)



add_executable(${PROJECT_NAME}
        CSockinit.h
        CSockinit.cpp
        main.cpp
)

