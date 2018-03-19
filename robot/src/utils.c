#include "utils.h"


inline void handle_errno(const char *msg, bool exit_program) {
  if (exit_program) {
    ZF_LOGF("%s: %s", msg, strerror(errno)); //log error message and description of errno
    abort();
  } else {
    ZF_LOGE("%s: %s", msg, strerror(errno)); //log error message and description of errno
  }
  return;
}

inline void handle_herrno(const char *msg, bool exit_program) {
  if (exit_program) {//if caller asked for it, exit program
    ZF_LOGF("%s: %s", msg, hstrerror(h_errno));
    abort();
  } else {
    ZF_LOGE("%s: %s", msg, hstrerror(h_errno));
  }
  return;
}

inline void handle_err(const char *msg, bool exit_program) {
  if (exit_program) {//if caller asked for it, exit program
    ZF_LOGF(msg);
    abort();
  } else {
    ZF_LOGE(msg);
  }
  return;
}
