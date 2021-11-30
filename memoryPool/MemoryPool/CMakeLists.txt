cmake_minimum_required(VERSION 3.0)
project(memoryPoolTest)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

aux_source_directory(. SRC_LIST)

add_executable(memoryPoolTest ${SRC_LIST})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)