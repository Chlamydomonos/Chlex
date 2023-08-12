cmake_minimum_required(VERSION 3.0)

project(chlex)

set(CMAKE_CXX_STANDARD 17)

file(GLOB LIB_SOURCES lib/src/*.cc)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC lib/include)