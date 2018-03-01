#add the main executable
add_executable (wrtcrobot utils.c signaling.c main.c)

target_link_libraries(wrtcrobot ev3dev-c cJSON)

install (TARGETS wrtcrobot DESTINATION .)
