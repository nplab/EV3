#include "signaling.h"

//socket for connection to signaling server
static int sigserv_sock_d;

struct hostent* get_hostinf(const char *host_name_or_addr){
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

wrtcr_rc sigserv_connect() {
  sigserv_sock_d = socket(AF_INET, SOCK_STREAM, 0); // open IPv4 TCP socket
  if (sigserv_sock_d < 0) {
    handle_errno("Could not create socket for connecting to signaling server",
                 true);
  }

  //get values for sig_server connection from config
  int port = 0;
  EOE(conf_get_int("sig_serv.port", &port), "Could not load signaling server port from config.")
  char *host_name_or_addr = NULL;
  EOE(conf_get_string("sig_serv.host", &host_name_or_addr), "Could not load signaling server host from config.");
  unsigned int room = 0;
  EOE(conf_get_int("sig_serv.room", (int *)&room), "Could not load signaling server room from config.");

  //set up address of signaling server 
  struct hostent *host_inf = get_hostinf(host_name_or_addr);
  if( host_inf == NULL){
    handle_herrno("Could not find host information for signaling server", true);
  }
  struct sockaddr_in sigserv_addr;
  sigserv_addr.sin_family = host_inf->h_addrtype;
  sigserv_addr.sin_addr = *(struct in_addr *)host_inf->h_addr_list[0];
  sigserv_addr.sin_port = htons(port);
  EONZ(connect(sigserv_sock_d, (struct sockaddr *)&sigserv_addr,
               sizeof(sigserv_addr)), "Socket could not connect to signaling server");

  //join room on signaling server
  room = htonl(room);
  ssize_t sent_len = send(sigserv_sock_d, &room, sizeof(unsigned int), 0);
  if (sent_len < 0) {
    handle_errno("Could not join room on signaling server", false);
  }

  //check for READY### message
  char *msg = NULL;
  EOE(sigserv_receive(&msg), "Could not get first message from signaling server");
  EONZ(strncmp(msg, "READY###", 8), "Signaling servers first message was not READY###");

  free(msg);
  return WRTCR_SUCCESS;
}

wrtcr_rc sigserv_send(char *msg) {
  wrtcr_rc rc = WRTCR_SUCCESS;
  char *buf = NULL;
  unsigned int buf_len;
  uint32_t msg_len = strlen(msg);

  //prepare send buffer
  buf_len=msg_len*sizeof(char)+sizeof(uint32_t);
  buf = (char *) malloc(buf_len);
  memcpy(buf, &msg_len, sizeof(uint32_t));
  memcpy(buf+sizeof(uint32_t), msg, msg_len*sizeof(char));

  //send and handle result
  ssize_t sent_len = send(sigserv_sock_d, buf, buf_len, 0);
  if (sent_len < 0) {
    handle_errno("Could not send message to signaling server", false);
    rc=WRTCR_FAILURE;
  } else if ((unsigned int)sent_len != msg_len + sizeof(uint32_t)) {
    handle_errno("Could not send complete message to signaling server", false);
    rc=WRTCR_FAILURE;
  } else {
    ZF_LOGD("Send message to signaling server: %s", msg);
  }

  free(buf);
  return rc;
}

wrtcr_rc sigserv_receive(char **msg) {
  uint8_t *recv_buf = NULL;
  uint32_t msg_len;

  ssize_t recv_len = recv(sigserv_sock_d, &msg_len, sizeof(msg_len), 0);
  if (recv_len != sizeof(msg_len)) {
    handle_errno("Failed to get length field of message from signaling server", false);
    return WRTCR_FAILURE;
  }
  msg_len = ntohl(msg_len);

  recv_buf = (uint8_t*)malloc(msg_len * sizeof(uint8_t));
  recv_len = recv(sigserv_sock_d, recv_buf, msg_len, 0);
  if (recv_len < 0 || (unsigned int)recv_len != msg_len) {
    handle_errno("Failed to get specified number of bytes from signaling server", false);
    free(recv_buf);
    return WRTCR_FAILURE;
  }
  *msg = (char*)malloc((recv_len+1)*sizeof(char)); //allocate memory for received message
  snprintf(*msg, recv_len+1, "%s", recv_buf); //move relevant message to the message buffer and add terminating \0

  free(recv_buf);
  return WRTCR_SUCCESS;
}

wrtcr_rc sigserv_disconnect() {
  if (close(sigserv_sock_d) < 0) {
    handle_errno("Could not close connection to signaling server", false);
  }
  return WRTCR_SUCCESS;
}
