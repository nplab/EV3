#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "config.h"
#include "utils.h"

const char *no_item_msg = "Could not find %s value for config key %s!\n";
const char *faulty_array_msg = "Array for config key %s did not contain only %s!\n";
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


//traverse JSON config tree to find specified item
cJSON* get_conf_item(char *key){
  //make copy of key, since strtok needs to modify it
  char *mod_key = (char*)malloc((strlen(key)+1)*sizeof(char));
  strcpy(mod_key, key);
  //start at the root of the config tree
  cJSON *cur_item = conf_root;
  //get first token from .-delimited config key
  char *cur_token = strtok(mod_key, ".");
  while(cur_token != NULL){ //while we haven't reached the end of the key
    cur_item = cJSON_GetObjectItem(cur_item, cur_token); //try to find an item whose name is the current token in the level below the current
    cur_token = strtok(NULL, "."); //get next token
  }
  return cur_item; //return item (might be NULL if none was found)
}

int conf_get_int(char *key, int *value){
  cJSON *item = get_conf_item(key); //get item or NULL
  if( item == NULL || !cJSON_IsNumber(item)){ //if we didn't get anything or it's not an integer return failure
    fprintf(stderr, no_item_msg, "integer", key);
    return WRTCR_FAILURE;
  }
  else{ //else return integer value of found item
    *value = item->valueint;
    return WRTCR_SUCCESS;
  }
}

int conf_get_string(char *key, char **value){
  cJSON *item = get_conf_item(key);//get item or NULL
  if( item == NULL || !cJSON_IsString(item)){//if we didn't get anything or it's not a string return failure
    fprintf(stderr, no_item_msg, "string", key);
    return WRTCR_FAILURE;
  }
  else{//else return string value of found item
    *value = item->valuestring;
    return WRTCR_SUCCESS;
  }
}

wrtcr_rc conf_get_string_array(char *key, char **array, int *length){
  cJSON *found_item = get_conf_item(key);
  if( found_item == NULL || !cJSON_IsArray(found_item)){//if we didn't get anything or it's not an array return failure
    fprintf(stderr, no_found_item_msg, "array", key);
    return wrtcr_rc.FAILURE;
  }
  else{//else write array and its length and return success
    *length = cJSON_GetArraySize(found_item);
    *array = (char*)malloc(*length*sizeof(char *));
    for(int i=0; i<*length; i++){ //Note: cJSON has a function that iterates over array efficiently, but these are short so it doesn't matter
      cJSON *entry = cJSON_GetArrayItem(found_item, i);
      if( !cJSON_IsString(entry)){ //check if all items are strings
        fprintf(stderr, faulty_array_msg, key, "string");
        return wrtcr_rc.FAILURE;
      }else{
        *array[i] = entry->valuestring;
      }
    }
    return wrtcr_rc.SUCCESS;
  }

}
