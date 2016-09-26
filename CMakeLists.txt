cmake_minimum_required(VERSION 2.8)

project(sabre_maestro)

set(CMAKE_CXX_FLAGS "-ggdb --std=c++11")

find_package(sdl2 REQUIRED)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${SDL2_INCLUDE_DIRS}
)

set(BUILD_SHARED_LIBS ON)

add_library(pololu_maestro
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MaestroDevice.cpp
)

add_executable(traxxas_joystick src/TraxxasMaxxJoystick.cpp src/DriveByWire.cpp)
target_link_libraries(traxxas_joystick pololu_maestro ${SDL2_LIBRARIES})
