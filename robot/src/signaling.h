#pragma once

int sigserv_connect();

int sigserv_send(char *msg);

int sigserv_receive();

int sigserv_disconnect();
