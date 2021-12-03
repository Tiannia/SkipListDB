cmake_minimum_required(VERSION 3.0)
project(memoryPoolTest_2rd)

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -g)

aux_source_directory(. SRC_LIST)

add_executable(memoryPoolTest_2rd ${SRC_LIST})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(memoryPoolTest_2rd pthread)