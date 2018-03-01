#include "utils.h"

inline void handle_errno(char *msg, bool exit_program) {
  perror(msg); //print error message and description of errno
  if (exit_program) {//if caller asked for it, exit program
    abort();
  }
  return;
}
