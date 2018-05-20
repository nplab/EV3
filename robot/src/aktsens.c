#include "aktsens.h"

//define tacho function map
typedef wrtcr_rc (*tacho_api_function)(uint8_t sn, cJSON *value);
typedef map_t(tacho_api_function) tacho_function_map_t;

//print on error of the robot set or get functions which return the number of bytes written or read
#define POSGE(rc, msg, fail) if(!(rc)){handle_err(msg, false); if(fail) return WRTCR_FAILURE;}
map_int_t port_map;
tacho_function_map_t tf_map;

wrtcr_rc set_up_function_maps();
wrtcr_rc tacho_stop_handler(uint8_t sn, cJSON *value);
wrtcr_rc tacho_run_forever_handler(uint8_t sn, cJSON *value);
wrtcr_rc tacho_run_to_rel_pos_handler(uint8_t sn, cJSON *value);
wrtcr_rc tacho_set_position_handler(uint8_t sn, cJSON *value);
wrtcr_rc tacho_set_stop_action_handler(uint8_t sn, cJSON *value);
wrtcr_rc tacho_get_state_handler(uint8_t sn, cJSON *value);
wrtcr_rc handle_sensor_value_request(char *port, uint8_t sn);
wrtcr_rc handle_sensor_set_mode(uint8_t sn, cJSON *mode_item);

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

  set_up_function_maps();

  return WRTCR_SUCCESS;
}

wrtcr_rc get_port_description(char **out_string){
  char port[8];
  cJSON *root = cJSON_CreateArray();
  cJSON *tempObj;
  map_iter_t iterator = map_iter(&port_map);
  const char *key;
  uint8_t sn;
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

wrtcr_rc set_up_function_maps(){
  map_init(&tf_map);
  map_set(&tf_map, "stop", tacho_stop_handler);
  map_set(&tf_map, "run-forever", tacho_run_forever_handler);
  map_set(&tf_map, "run-to-rel-position", tacho_run_to_rel_pos_handler);
  map_set(&tf_map, "set-position", tacho_set_position_handler);
  map_set(&tf_map, "set-stop_action", tacho_set_stop_action_handler);
  map_set(&tf_map, "get-state", tacho_get_state_handler);

  return WRTCR_SUCCESS;
}

wrtcr_rc handle_tacho_message(char *port, cJSON *message){
  uint8_t sn = *map_get(&port_map, port);
  cJSON *mode_item = cJSON_GetObjectItem(message, "mode");
  char *mode = cJSON_GetStringValue(mode_item);
  if(mode == NULL){
    handle_err("Message malformed, no mode", false);
    return WRTCR_FAILURE;
  }
  cJSON *value_item = cJSON_GetObjectItem(message, "value");
  tacho_api_function func = *map_get(&tf_map, mode);

  func(sn, value_item);
  return WRTCR_SUCCESS;
}

wrtcr_rc handle_sensor_message(char *port, cJSON *message){
  uint8_t sn = *map_get(&port_map, port);
  cJSON *mode_item = cJSON_GetObjectItem(message, "mode");
  if( !mode_item ){
    return handle_sensor_value_request(port, sn);
  } else {
    return handle_sensor_set_mode(sn, mode_item);
  }
  return WRTCR_SUCCESS;
}

wrtcr_rc tacho_stop_handler(uint8_t sn, cJSON *value){
  (void) value;
  set_tacho_command_inx(sn, TACHO_STOP);
  return WRTCR_SUCCESS;
}

wrtcr_rc tacho_run_forever_handler(uint8_t sn, cJSON *value){
  int speed, max_speed;
  get_tacho_max_speed(sn, &max_speed);
  if(!cJSON_IsNumber(value)){
    return WRTCR_FAILURE;
  }
  speed = value->valueint*max_speed/100; //convert speed percentage to motor value
  set_tacho_speed_sp(sn, speed);
  set_tacho_command_inx(sn, TACHO_RUN_FOREVER);
  return WRTCR_SUCCESS;
}

wrtcr_rc tacho_run_to_rel_pos_handler(uint8_t sn, cJSON *value){
  int pos;
  if(!cJSON_IsNumber(value)){
    return WRTCR_FAILURE;
  }
  pos = value->valueint;
  set_tacho_position_sp(sn, pos);
  set_tacho_command_inx(sn, TACHO_RUN_TO_REL_POS);
  return WRTCR_SUCCESS;
}

wrtcr_rc tacho_set_position_handler(uint8_t sn, cJSON *value){
  int pos;
  if(!cJSON_IsNumber(value)){
    return WRTCR_FAILURE;
  }
  pos = value->valueint;
  set_tacho_position(sn, pos);
  return WRTCR_SUCCESS;
}

wrtcr_rc tacho_set_stop_action_handler(uint8_t sn, cJSON *value){
  char *stop_action = cJSON_GetStringValue(value);
  if(stop_action == NULL){
    return WRTCR_FAILURE;
  }
  set_tacho_stop_action(sn, stop_action);
  return WRTCR_SUCCESS;
}

wrtcr_rc tacho_get_state_handler(uint8_t sn, cJSON *value){
  (void*) value;
  char state[10];
  char port[5];
  char msg[40];
  get_tacho_state(sn, state, sizeof(state));

  snprintf(msg, sizeof(msg),
           "{\"port\": \"%s\", \"state\": \"%s\"}", 
           &(ev3_tacho_port_name(sn, port)[3]), state);

  EOE(send_message_on_api_channel(msg), "Could not send tacho motor state message");

  return WRTCR_SUCCESS;
}

wrtcr_rc handle_sensor_value_request(char *port, uint8_t sn){
  int i, buf[8];
  char *msg;
  for(i=0; i<8; i++){
    if(get_sensor_value(i, sn, &buf[i]) < 1){ //if we can get the value, leave loop
      break;
    }
  }
  //create JSON 
  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "port", port);
  cJSON *values = cJSON_CreateIntArray(buf, i);
  cJSON_AddItemToObject(root, "values", values);

  msg = cJSON_Print(root);
  POE(send_message_on_api_channel(msg), "Could not send sensor value message");

  cJSON_Delete(root);
  free(msg);
  return WRTCR_SUCCESS;
}

wrtcr_rc handle_sensor_set_mode(uint8_t sn, cJSON *mode_item){
  char *mode = cJSON_GetStringValue(mode_item);
  if( strlen(mode) != set_sensor_mode(sn, mode)){ //set mode and check that it worked
    return WRTCR_FAILURE;
  } else {
    return WRTCR_SUCCESS;
  }
}
