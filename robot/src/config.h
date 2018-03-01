#pragma once

#include <cJSON.h>

//read and parse the JSON config file
int read_config(char *filename);

//delete parsed configuration
int delete_config();

//get integer <value> for key <key>
int conf_get_int(char *key, int *value);

//get char pointer <value> for key <key> 
int conf_get_string(char *key, char **value);
