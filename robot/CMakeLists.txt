#The basics
cmake_minimum_required (VERSION 3.6)
project (WebRTCDemo-Robot)

#version numbers
set (WebRTCDemo-Robot_VERSION_MAJOR 0)
set (WebRTCDemo-Robot_VERSION_MINOR 1)

#compilation settings
set (CMAKE_C_FLAGS "-Wall -Wextra -Werror -std=c11")
set (CMAKE_C_FLAGS_DEBUG "-g")
set (CMAKE_C_FLAGS_RELEASE "-O3")

#directories
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory (src)

