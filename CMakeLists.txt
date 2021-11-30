cmake_minimum_required(VERSION 3.0)
project(KVDB)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(. SRC_LIST)

add_executable(SkipListDB ${SRC_LIST})

#include_directories(${PROJECT_SOURCE_DIR})

target_link_libraries(SkipListDB pthread)