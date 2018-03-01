#pragma once

#include <cJSON.h>

//read and parse the JSON config file
int read_config(char *filename);

//get integer <value> for key <key> in section <section>
int get_int(char *section, char *key, int *value);

//get char pointer <value> for key <key> in section <section>
int get_string(char *section, char *key, char *value);
