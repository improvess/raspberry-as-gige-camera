cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME rpiasgige_client)
project(${PROJECT_NAME} CXX)

if (NOT CMAKE_BUILD_TYPE) 
  set(CMAKE_BUILD_TYPE RelWithDebInfo) 
endif() 

message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

# C++11 is mandatory
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
include_directories(/usr/include/)

# installed libraries

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# building library

file(GLOB SOURCES_LIB "${PROJECT_SOURCE_DIR}/src/lib/*.cpp")
add_library(${PROJECT_NAME}_lib ${SOURCES_LIB})

# building app

file(GLOB APP_SOURCES 
  ${PROJECT_SOURCE_DIR}/src/backward.cpp 
  ${PROJECT_SOURCE_DIR}/src/main.cpp )
add_executable(${PROJECT_NAME} ${APP_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -pedantic)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# linking stacktracing libs. See https://github.com/bombela/backward-cpp#libbfd-from-the-gnubinutils
target_link_libraries(${PROJECT_NAME} bfd dl)

# linking OpenCV
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} -pthread)
