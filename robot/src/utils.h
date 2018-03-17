#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

//configure logging library
#define ZF_LOG_LEVEL ZF_LOG_DEBUG
#define ZF_LOG_SRCLOC ZF_LOG_SRCLOC_NONE
#include <zf_log.h>

typedef enum WebRTCRobot_ReturnCode {
  WRTCR_SUCCESS,
  WRTCR_FAILURE
} wrtcr_rc;

//on non-successful wrtcr_rc log error message without printing errno and exit
#define EOE(rc, msg) if( rc != WRTCR_SUCCESS){handle_err(msg, true);}
//on non-successful wrtcr_rc log error message without printing errno and do not exit
#define POE(rc, msg) if( rc != WRTCR_SUCCESS){handle_err(msg, false);}
//on non-zero return code log error message, print errno and exit
#define EONZ(rc, msg) if( rc != 0){handle_errno(msg, true);}
//on non-zero return code log error message, print errno and exit
#define PONZ(rc, msg) if( rc != 0){handle_errno(msg, false);}
//on null return code log error message, print errno and exit
#define EON(rc, msg) if( rc == NULL){handle_errno(msg, true);}
//on null return code log error message, print errno and exit
#define PON(rc, msg) if( rc == NULL){handle_errno(msg, false);}
//on non-successful rawrtc return code log error message without printing errno and exit
#define EORE(rc, msg) if( rc != RAWRTC_CODE_SUCCESS){handle_err(msg, true);}
//on non-successful rawrtc return code log error message without printing errno and do not exit
#define PORE(rc, msg) if( rc != RAWRTC_CODE_SUCCESS){handle_err(msg, false);}

extern int errno;
extern int h_errno;

//print message, followed by string representation of current errno, optionally exit program
extern void handle_errno(const char *msg, bool exit_program);

//print message, followed by string representation of current h_errno, optionally exit program
extern void handle_herrno(const char *msg, bool exit_program);

//print message, optionally exit program
extern void handle_err(const char *msg, bool exit_program);
