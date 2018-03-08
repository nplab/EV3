#add the main executable
add_executable (wrtcrobot setup.c config.c utils.c signaling.c main.c)

target_link_libraries(wrtcrobot ev3dev-c cJSON zflog)

install (TARGETS wrtcrobot DESTINATION .)
