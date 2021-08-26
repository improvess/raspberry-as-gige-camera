cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME rpiasgige)
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

# building usb-test app

file(GLOB APP_SOURCES 
  ${PROJECT_SOURCE_DIR}/src/backward.cpp 
  ${PROJECT_SOURCE_DIR}/extras/usb_camera_test.cpp )
add_executable(usb_test_${PROJECT_NAME} ${APP_SOURCES})

target_compile_options(usb_test_${PROJECT_NAME} PRIVATE -pedantic)
target_link_libraries(usb_test_${PROJECT_NAME} ${PROJECT_NAME}_lib)
target_link_libraries(usb_test_${PROJECT_NAME} bfd dl)
target_link_libraries(usb_test_${PROJECT_NAME} ${OpenCV_LIBS} -pthread)

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message(STATUS "Building for architecture: ${ARCHITECTURE}")

# specific libs for raspberry pi 64 bits and GCC 10
if( ${ARCHITECTURE} STREQUAL "aarch64" )
  target_link_libraries(${PROJECT_NAME} 
    /home/pi/gcc-10.2.0/obj/stage1-aarch64-unknown-linux-gnu/libstdc++-v3/src/.libs/libstdc++.a)
endif()

option(BUILD_TESTS "Build the tests" OFF)

if(BUILD_TESTS)

  set(PROJECT_TEST_NAME test_${PROJECT_NAME})

  include(${PROJECT_SOURCE_DIR}/../libs/googletest/install.txt)

  include_directories(${PROJECT_SOURCE_DIR}/tests/include)

  file(GLOB TEST_SRC_FILES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
  add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

  # specific libs for raspberry pi 64 bits and GCC 10
  if( ${ARCHITECTURE} STREQUAL "aarch64" )
  target_link_libraries(${PROJECT_TEST_NAME}
    /home/pi/gcc-10.2.0/obj/stage1-aarch64-unknown-linux-gnu/libstdc++-v3/src/.libs/libstdc++.a)
  endif()

  target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME}_lib gtest_main ${OpenCV_LIBS} -pthread)

  add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_TEST_NAME})

endif()