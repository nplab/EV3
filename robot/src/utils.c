#include "utils.h"

inline void handle_errno(char *msg, bool exit_program) {
  perror(msg);
  if (exit_program) {
    exit(EXIT_FAILURE);
  }
  return;
}
