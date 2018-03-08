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

#define WRTCR_SUCCESS 0
#define WRTCR_FAILURE -1

//on non-successful wrtcr_rc log error message without printing errno and exit
#define EOE(rc, msg) if( rc != WRTCR_SUCCESS){handle_err(msg, true);}
//on non-successful wrtcr_rc log error message without printing errno and do not exit
#define POE(rc, msg) if( rc != WRTCR_SUCCESS){handle_err(msg, false);}
//on non-zero return code log error message, print errno and exit
#define EONZ(rc, msg) if( rc != 0){handle_errno(msg, true);}
//on non-zero return code log error message, print errno and exit
#define PONZ(rc, msg) if( rc != 0){handle_errno(msg, false);}
//on non-successful rawrtc return code log error message without printing errno and exit
#define EORE(rc, msg) if( rc != RAWRTC_SUCCESS){handle_err(msg, true);}
//on non-successful rawrtc return code log error message without printing errno and do not exit
#define PORE(rc, msg) if( rc != RAWRTC_SUCCESS){handle_err(msg, false);}

extern int errno;
extern int h_errno;

//print message, followed by string representation of current errno, optionally exit program
extern void handle_errno(char *msg, bool exit_program);

//print message, followed by string representation of current h_errno, optionally exit program
extern void handle_herrno(char *msg, bool exit_program);

//print message, optionally exit program
extern void handle_err(char *msg, bool exit_program);
