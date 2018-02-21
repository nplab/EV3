#define _BSD_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
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

  return WRTCR_SUCCESS;
}

int sigserv_send() { return WRTCR_SUCCESS; }

int sigserv_receive() { return WRTCR_SUCCESS; }

int sigserv_disconnect() {
  if(close(sigserv_sock_d) < 0){
    handle_errno("Could not close connection to signaling server", false);
  }
  return WRTCR_SUCCESS;
}
