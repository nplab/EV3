#The basics
cmake_minimum_required (VERSION 3.6)
project (WebRTCDemo-Robot)

#version numbers
set (WebRTCDemo-Robot_VERSION_MAJOR 0)
set (WebRTCDemo-Robot_VERSION_MINOR 1)

#directories
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory (src)
