#include "aktsens.h"

//print on error of the robot set or get functions which return the number of bytes written or read
#define POSGE(rc, msg, fail) if(!(rc)){handle_err(msg, false); if(fail) return WRTCR_FAILURE;}
map_int_t port_map;

wrtcr_rc setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not initialize robot library", true);
  }
  if( ev3_sensor_init() < 1){
    handle_err("Could not initialize sensors", true);
  }
  map_init(&port_map);
  char port[8];
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
    if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
      ev3_sensor_port_name( i, port );
      map_set(&port_map, &(port[2]), i);
    }
  }
  if( ev3_tacho_init() < 1){
    handle_err("Could not initialize motors", true);
  }
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
    if ( ev3_tacho[ i ].type_inx != TACHO_TYPE__NONE_ ) {
      ev3_tacho_port_name( i, port );
      map_set(&port_map, &(port[3]), i);
    }
  }
  return WRTCR_SUCCESS;
}

wrtcr_rc get_port_description(char **out_string){
  char port[8];
  cJSON *root = cJSON_CreateArray();
  cJSON *tempObj;
  map_iter_t iterator = map_iter(&port_map);
  const char *key;
  int sn;
  while( (key = map_next(&port_map, &iterator))){
    sn = *map_get(&port_map, key);
    tempObj = cJSON_CreateObject();
    if( *key<'A' ){
      ev3_sensor_port_name(sn, port);
      cJSON_AddStringToObject(tempObj, "type", ev3_sensor_type( ev3_sensor[sn].type_inx ));
    } else {
      ev3_tacho_port_name(sn, port);
      cJSON_AddStringToObject(tempObj, "type", ev3_tacho_type( ev3_tacho[sn].type_inx ));
    }
    cJSON_AddStringToObject(tempObj, "port", port + strlen(port) -1);
    cJSON_AddItemToArray(root, tempObj);
  }
  *out_string = cJSON_Print(root);
  cJSON_Delete(root);
  return WRTCR_SUCCESS;
}

wrtcr_rc handle_tacho_message(char port, cJSON *message){
  
  return WRTCR_SUCCESS;
}

wrtcr_rc handle_sensor_message(char port, cJSON *message){
  return WRTCR_SUCCESS;
}
