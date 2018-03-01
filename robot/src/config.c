#include <stdio.h>
#include <assert.h>

#include "config.h"
#include "utils.h"

static cJSON *root = NULL;

int read_config(char *filename){
  //open file
  FILE *fd = fopen(filename, "r");
  if( fd == NULL ){
    handle_errno("Could not open config file", true);
  }

  //read in whole file as one string
  char *conf_string = NULL;
  size_t cs_len = 0;
  ssize_t in_len = getdelim(&conf_string, &cs_len, '\0', fd);
  if(in_len == -1){
    handle_errno("Could not read config file", true);
  }

  //clean up
  fclose(fd);
  free(conf_string);

  return WRTCR_SUCCESS;
}

int delete_config(){
  cJSON_Delete(root);
  return WRTCR_SUCCESS;
}

int get_int(char *key, int *value){
  return WRTCR_SUCCESS;
}

int get_string(char *key, char *value){
  return WRTCR_SUCCESS;
}
