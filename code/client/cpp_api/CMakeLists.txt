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

# building basic example

file(GLOB BASIC_EXAMPLE_SOURCES 
  ${PROJECT_SOURCE_DIR}/src/backward.cpp 
  ${PROJECT_SOURCE_DIR}/examples/basic.cpp )
add_executable(basic_example ${BASIC_EXAMPLE_SOURCES})
target_compile_options(basic_example PRIVATE -pedantic)
target_link_libraries(basic_example bfd dl)
target_link_libraries(basic_example ${OpenCV_LIBS} -pthread)

# building basic example

file(GLOB CHECK_CAMERA_SYNC 
  ${PROJECT_SOURCE_DIR}/src/backward.cpp 
  ${PROJECT_SOURCE_DIR}/examples/check_camera_synchronization.cpp )
add_executable(check_camera_synchronization ${CHECK_CAMERA_SYNC})
target_compile_options(check_camera_synchronization PRIVATE -pedantic)
target_link_libraries(check_camera_synchronization bfd dl)
target_link_libraries(check_camera_synchronization ${OpenCV_LIBS} -pthread)
