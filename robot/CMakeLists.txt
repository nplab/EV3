#The basics
cmake_minimum_required (VERSION 3.6)
project (WebRTCDemo-Robot)

#version numbers
set (WebRTCDemo-Robot_VERSION_MAJOR 0)
set (WebRTCDemo-Robot_VERSION_MINOR 1)

### external libraries
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
set_directory_properties(PROPERTIES EP_BASE "lib")

#ev3dev-c
ExternalProject_Add(ev3dev-c_project
  GIT_REPOSITORY https://github.com/JesApp/ev3dev-c.git
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND cd source/ev3 && make
  INSTALL_COMMAND cd source/ev3 && make D_INSTALL=<INSTALL_DIR> D_INSTALL_H_FROM=<SOURCE_DIR>/source/ev3 install
)

ExternalProject_Get_Property(ev3dev-c_project install_dir)
add_library(ev3dev-c STATIC IMPORTED)
set_property(TARGET ev3dev-c PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libev3dev-c.a)
add_dependencies(ev3dev-c ev3dev-c_project)
include_directories(${install_dir}/include)

#cJSON
ExternalProject_Add(cJSON_project
  GIT_REPOSITORY https://github.com/DaveGamble/cJSON.git
  GIT_TAG v1.7.3
  CMAKE_ARGS -DENABLE_CJSON_TEST=Off -DBUILD_SHARED_LIBS=Off -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(cJSON_project install_dir)
add_library(cJSON STATIC IMPORTED)
set_property(TARGET cJSON PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libcjson.a)
add_dependencies(cJSON cJSON_project)
include_directories(${install_dir}/include/cjson/)

#compilation settings
set (CMAKE_C_FLAGS "-Wall -Wextra -Werror -std=c11")
set (CMAKE_C_FLAGS_DEBUG "-g")
set (CMAKE_C_FLAGS_RELEASE "-O3")

#directories
add_subdirectory (src)
