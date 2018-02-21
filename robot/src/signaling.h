#pragma once

int sigserv_connect();

int sigserv_send(char *msg);

int sigserv_receive(char **msg);

int sigserv_disconnect();
