#pragma once

//connect to signaling server based on config file
int sigserv_connect();

//send a string to the signaling server in the current room
int sigserv_send(char *msg);

//receive a string message from the signaling server in the current room
int sigserv_receive(char **msg);

//disconnect from the signaling server
int sigserv_disconnect();
