#pragma once

#define _BSD_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "utils.h"
#include "config.h"

//connect to signaling server based on config file
wrtcr_rc sigserv_connect();

//send a string to the signaling server in the current room
wrtcr_rc sigserv_send(char *msg);

//receive a string message from the signaling server in the current room
wrtcr_rc sigserv_receive(char **msg);

//disconnect from the signaling server
wrtcr_rc sigserv_disconnect();
