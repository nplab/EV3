#include "utils.h"

inline void handle_errno(char *msg, bool exit_program) {
  perror(msg); //print error message and description of errno
  if (exit_program) {//if caller asked for it, exit program
    abort();
  }
  return;
}

inline void handle_herrno(char *msg, bool exit_program) {
  herror(msg); //print error message and description of errno
  if (exit_program) {//if caller asked for it, exit program
    abort();
  }
  return;
}

inline void handle_err(char *msg, bool exit_program) {
  fprintf(stderr, "%s.", msg); //print error message and description of errno
  if (exit_program) {//if caller asked for it, exit program
    fprintf(stderr, " Quitting!"); //print error message and description of errno
    abort();
  } else {
    fprintf(stderr, "\n"); //print error message and description of errno
  }
  return;
}
