#pragma once

#include <cJSON.h>

//read and parse the JSON config file
int read_config(char *filename);

//get integer <value> for key <key>
int get_int(char *key, int *value);

//get char pointer <value> for key <key> 
int get_string(char *key, char *value);
