#add the main executable
add_executable (wrtcrobot data.c setup.c config.c utils.c signaling.c main.c)

target_link_libraries(wrtcrobot ev3dev-c cJSON zflog ${rawrtc_DEP_LIBRARIES})

install (TARGETS wrtcrobot DESTINATION .)
