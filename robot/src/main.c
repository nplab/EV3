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

  sigserv_disconnect();

  return 0;
}
