#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "config.h"
#include "utils.h"

static cJSON *conf_root = NULL;

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

  //parse config
  conf_root = cJSON_Parse(conf_string);
  if( conf_root == NULL){
    const char *problem = cJSON_GetErrorPtr();
    if( problem != NULL ){
      fprintf(stderr, "Parsing of JSON in config file failed  before: %s\n", problem);
      abort();
    }
  }

  //clean up
  fclose(fd);
  free(conf_string);

  return WRTCR_SUCCESS;
}

int delete_config(){
  cJSON_Delete(conf_root);
  return WRTCR_SUCCESS;
}

cJSON* get_conf_item(char *key){
  cJSON *cur_item = conf_root;
  char *cur_token = strtok(key, '.');
  while(cur_token != NULL){
    cur_item = cJSON_getObjectItem(cur_item, cur_token);
    char *cur_token = strtok(key, '.');
  }
  return cur_item;
}

int get_int(char *key, int *value){
  cJSON *item = get_conf_item(key);
  if( item == NULL || !cJSON_IsNumber(item)){
    return WRTCR_FAILURE;
  }
  else{
    *value = item->valueint;
    return WRTCR_SUCCESS;
  }
}

int get_string(char *key, char **value){
  cJSON *item = get_conf_item(key);
  if( item == NULL || !cJSON_IsString(item)){
    return WRTCR_FAILURE;
  }
  else{
    *value = item->valuestring;
    return WRTCR_SUCCESS;
  }
}
