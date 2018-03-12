#The basics
cmake_minimum_required (VERSION 3.6)
project (WebRTCDemo-Robot)

if (${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    message(STATUS "enabling verbose outout")
    set(CMAKE_VERBOSE_MAKEFILE on)
endif ()

#version numbers
set (WebRTCDemo-Robot_VERSION_MAJOR 0)
set (WebRTCDemo-Robot_VERSION_MINOR 1)

### external libraries
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FindPkgConfig.cmake)
set_directory_properties(PROPERTIES EP_BASE "lib")

#ev3dev-c
ExternalProject_Add(ev3dev-c_project
  GIT_REPOSITORY https://github.com/JesApp/ev3dev-c.git
  UPDATE_DISCONNECTED 1
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

#zf_log
ExternalProject_Add(zflog_project 
  GIT_REPOSITORY https://github.com/wonder-mice/zf_log.git
  GIT_TAG v0.4.0
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(zflog_project install_dir)
add_library(zflog STATIC IMPORTED)
set_property(TARGET zflog PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libzf_log.a)
add_dependencies(zflog zflog_project)
include_directories(${install_dir}/include/)

## find RawRTC dependencies
#librew NOTE: This needs to be before libre, or it produces linker errors
pkg_check_modules(LIB_REW REQUIRED "librew >= 0.5.0")
include_directories(${LIB_REW_STATIC_INCLUDE_DIRS})
link_directories(${LIB_REW_STATIC_LIBRARY_DIRS})
list(APPEND rawrtc_DEP_LIBRARIES ${LIB_REW_STATIC_LIBRARIES})

#libre
pkg_check_modules(LIB_RE REQUIRED "libre >= 0.5.7")
include_directories(${LIB_RE_STATIC_INCLUDE_DIRS})
link_directories(${LIB_RE_STATIC_LIBRARY_DIRS})
list(APPEND rawrtc_DEP_LIBRARIES ${LIB_RE_STATIC_LIBRARIES})

#usrsctp
find_library(LIB_USRSCTP_LIBRARIES NAMES usrsctp.a libusrsctp.a )
find_path(LIB_USRSCTP_INCLUDE_DIRS usrsctp.h)
get_filename_component(LIB_USRSCTP_LIBRARY_DIRS ${LIB_USRSCTP_LIBRARIES} DIRECTORY)
include_directories(${LIB_USRSCTP_INCLUDE_DIRS})
link_directories(${LIB_USRSCTP_LIBRARY_DIRS})
list(APPEND rawrtc_DEP_LIBRARIES ${LIB_USRSCTP_LIBRARIES})

#find rawrtc and make sure we are linking to the static lib
pkg_check_modules(LIB_RAWRTC REQUIRED "librawrtc >= 0.2.1")
include_directories(${LIB_RAWRTC_STATIC_INCLUDE_DIRS})
link_directories(${LIB_RAWRTC_STATIC_LIBRARY_DIRS})
find_library(rawrtc_static_lib rawrtc-static)
message(STATUS "Deps ${rawrtc_DEP_LIBRARIES}")

#compilation settings
set (CMAKE_C_FLAGS "-Wall -Wextra -std=c11")
set (CMAKE_C_FLAGS_DEBUG "-ggdb -O0")
set (CMAKE_C_FLAGS_RELEASE "-O3 -Werror")

#directories
add_subdirectory (src)
