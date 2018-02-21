#define _BSD_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "signaling.h"
#include "utils.h"

static int sigserv_sock_d;

int sigserv_connect() {
  sigserv_sock_d = socket(AF_INET, SOCK_STREAM, 0); // open IPv4 TCP socket
  if (sigserv_sock_d < 0) {
    handle_errno("Could not create socket for connecting to signaling server",
                 true);
  }

  struct sockaddr_in sigserv_addr;
  sigserv_addr.sin_family = AF_INET;
  sigserv_addr.sin_port = htons(5001);
  if (inet_aton("192.168.0.11", &(sigserv_addr.sin_addr)) == 0) {
    handle_errno("Could not parse given IP address", true);
  }

  if (connect(sigserv_sock_d, (struct sockaddr *)&sigserv_addr,
              sizeof(sigserv_addr)) < 0) {
    handle_errno("Socket could not connect to signaling server", true);
  }

  unsigned int room_nr = htonl(822738944);
  ssize_t sent_len = send(sigserv_sock_d, &room_nr, sizeof(unsigned int), 0);
  if (sent_len < 0) {
    handle_errno("Could not join room on signaling server", false);
  } 

  char recv_buf[8];
  ssize_t recv_len = recv(sigserv_sock_d, &recv_buf, sizeof(recv_buf), 0);
  if (recv_len < 0) {
    handle_errno("Did not get READY### message from signaling server", true);
  } 
  if( !strncmp(recv_buf, "READY###", recv_len) ){
    handle_errno("Signaling servers first message was not READY###", true);
  }

  return WRTCR_SUCCESS;
}

int sigserv_send(char *msg) {
  ssize_t sent_len = send(sigserv_sock_d, msg, strlen(msg) + 1, 0);
  if (sent_len < 0) {
    handle_errno("Could not send message to signaling server", false);
    return WRTCR_FAILURE;
  } else if ((unsigned int)sent_len != strlen(msg) + 1) {
    handle_errno("Could not send complete message to signaling server", false);
    return WRTCR_FAILURE;
  }
  return WRTCR_SUCCESS;
}

int sigserv_receive() {
  return WRTCR_SUCCESS; }

int sigserv_disconnect() {
  if (close(sigserv_sock_d) < 0) {
    handle_errno("Could not close connection to signaling server", false);
  }
  return WRTCR_SUCCESS;
}
