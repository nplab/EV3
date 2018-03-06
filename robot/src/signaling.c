#define _BSD_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "signaling.h"
#include "utils.h"
#include "config.h"

//socket for connection to signaling server
static int sigserv_sock_d;

struct hostent* get_hostinf(char *host_name_or_addr){
  struct in_addr ip4;
  struct in6_addr ip6;
  struct hostent *host = NULL;
  if (inet_pton(AF_INET, host_name_or_addr, &ip4) != 0) { //if it's an ipv4 address
    host = gethostbyaddr((const void*)&ip4, sizeof(ip4), AF_INET);
  } else if(inet_pton(AF_INET6, host_name_or_addr, &ip6) != 0){ // if it's an ipv6 address
    host = gethostbyaddr((const void*)&ip6, sizeof(ip6), AF_INET6);
  } else{ //if it's neither, hope it's a hostname
    host = gethostbyname(host_name_or_addr);
  }
  return host;
}

int sigserv_connect() {
  sigserv_sock_d = socket(AF_INET, SOCK_STREAM, 0); // open IPv4 TCP socket
  if (sigserv_sock_d < 0) {
    handle_errno("Could not create socket for connecting to signaling server",
                 true);
  }

  //get values for sig_server connection from config
  int port = 0;
  if(conf_get_int("sig_serv.port", &port) != WRTCR_SUCCESS){
    abort();
  }
  char *host_name_or_addr = NULL;
  if(conf_get_string("sig_serv.host", &host_name_or_addr) != WRTCR_SUCCESS){
    abort();
  }
  unsigned int room = 0;
  if(conf_get_int("sig_serv.room", (int *)&room) != WRTCR_SUCCESS){
    abort();
  }

  //set up address of signaling server 
  struct hostent *host_inf = get_hostinf(host_name_or_addr);
  if( host_inf == NULL){
    handle_herrno("Could not find host information for signaling server", true);
  }
  struct sockaddr_in sigserv_addr;
  sigserv_addr.sin_family = host_inf->h_addrtype;
  sigserv_addr.sin_addr = *(struct in_addr *)host_inf->h_addr_list[0];
  sigserv_addr.sin_port = htons(port);
  if (connect(sigserv_sock_d, (struct sockaddr *)&sigserv_addr,
              sizeof(sigserv_addr)) < 0) {
    handle_errno("Socket could not connect to signaling server", true);
  }

  //join room on signaling server
  room = htonl(room);
  ssize_t sent_len = send(sigserv_sock_d, &room, sizeof(unsigned int), 0);
  if (sent_len < 0) {
    handle_errno("Could not join room on signaling server", false);
  }

  //check for READY### message
  char recv_buf[16];
  ssize_t recv_len = recv(sigserv_sock_d, &recv_buf, sizeof(recv_buf), 0);
  if (recv_len < 0) {
    handle_errno("Did not get READY### message from signaling server", true);
  }
  if (!strncmp(recv_buf, "READY###", recv_len)) {
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

int sigserv_receive(char **msg) {
  uint8_t recv_buf[2047]; //create a (hopefully) large enough buffer (2047 so resulting msg is at most 2048 byte long)
  ssize_t recv_len = recv(sigserv_sock_d, &recv_buf, sizeof(recv_buf), 0);
  if (recv_len < 0) {
    handle_errno("Failed to receive message from signaling server", false);
    return WRTCR_FAILURE;
  }
  *msg = (char*)malloc((recv_len+1)*sizeof(char)); //allocate memory for received message
  snprintf(*msg, recv_len, "%s", recv_buf); //move relevant message to the message buffer and add terminating \0

  return WRTCR_SUCCESS;
}

int sigserv_disconnect() {
  if (close(sigserv_sock_d) < 0) {
    handle_errno("Could not close connection to signaling server", false);
  }
  return WRTCR_SUCCESS;
}
