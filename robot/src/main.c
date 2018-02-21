#include <stdio.h>

#include "signaling.h"
#include "utils.h"

int main(int argc, const char *argv[]) {
  printf("%d: ", argc);
  for (int i; i < argc; i++) {
    printf("%s", argv[i]);
  }

  sigserv_connect();

  return 0;
}
