cmake_minimum_required(VERSION 2.8)

project(sabre_maestro)

set(CMAKE_CXX_FLAGS "-ggdb --std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(BUILD_SHARED_LIBS ON)

add_library(pololu_maestro
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MaestroDevice.cpp
)

add_executable(maestro_test src/MaestroTestClient.cpp)
target_link_libraries(maestro_test pololu_maestro)
