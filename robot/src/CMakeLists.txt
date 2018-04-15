#add the main executable
add_executable (wrtcrobot default_handlers.c data.c config.c utils.c signaling.c main.c aktsens.c)

target_link_libraries(wrtcrobot ev3dev-c cJSON zflog ${rawrtc_static_lib} ${rawrtc_DEP_LIBRARIES})

install (TARGETS wrtcrobot DESTINATION .)
