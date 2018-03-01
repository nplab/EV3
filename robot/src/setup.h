#pragma once

#include <unistd.h>

#include "config.h"
#include "signaling.h"
#include "utils.h"

//set up the software -> read config, connect to signaling, connect to website, initialize robot
int setup(int argc, char *argv[]);

//shut down connections, remove data structures
int teardown();
