#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define WRTCR_SUCCESS 0
#define WRTCR_FAILURE -1

extern int errno;

//print message, followed by string representation of current errno, optionally exit program
extern void handle_errno(char *msg, bool exit_program);

//print message, followed by string representation of current h_errno, optionally exit program
extern void handle_herrno(char *msg, bool exit_program);

//print message, optionally exit program
extern void handle_err(char *msg, bool exit_program);
