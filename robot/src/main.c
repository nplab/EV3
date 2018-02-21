#include <stdio.h>

#include "signaling.h"
#include "utils.h"

int main(int argc, const char *argv[]) {
  printf("%d: ", argc);
  for (int i; i < argc; i++) {
    printf("%s", argv[i]);
  }
  printf("\n");

  sigserv_connect();

  char *msg;
  if(sigserv_receive(&msg) == WRTCR_SUCCESS)
    printf("%s\n", msg);

  sigserv_disconnect();

  return 0;
}
