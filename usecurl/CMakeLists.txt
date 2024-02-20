cmake_minimum_required(VERSION 3.20)

set(VCPKG_ROOT "D:/kerui/vcpkg-master/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")

set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT})

project(useCurl)

find_package(CURL REQUIRED)
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE CURL::libcurl)


