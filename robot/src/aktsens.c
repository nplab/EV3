#include "aktsens.h"

//print on error of the robot set or get functions which return the number of bytes written or read
#define POSGE(rc, msg, fail) if(!(rc)){handle_err(msg, false); if(fail) return WRTCR_FAILURE;}
#define NR_OF_PORTS 8
static uint8_t ports[NR_OF_PORTS];

unsigned int get_port_index(char port);
static inline wrtcr_rc add_port(const char port, const uint8_t sequence_number);

wrtcr_rc setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not initialize robot library", true);
  }
  for(int i=0; i<NR_OF_PORTS; i++){
    ports[i] = 255;
  }
  if( ev3_sensor_init() < 1){
    handle_err("Could not initialize sensors", true);
  }
  char port[ 8 ];
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
    if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
      ev3_sensor_port_name( i, port );
      EOE(add_port(port[2], i), "Could not add found sensor to port array");
    }
  }
  if( ev3_tacho_init() < 1){
    handle_err("Could not initialize motors", true);
  }
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
    if ( ev3_tacho[ i ].type_inx != TACHO_TYPE__NONE_ ) {
      ev3_tacho_port_name( i, port );
      EOE(add_port(port[3], i), "Could not add found sensor to port array");
    }
  }
  return WRTCR_SUCCESS;
}

static inline wrtcr_rc add_port(const char port, const uint8_t sequence_number){
  ports[get_port_index(port)] = sequence_number;
  return WRTCR_SUCCESS;
}

wrtcr_rc get_port_description(char **out_string){
  char port[8];
  cJSON *root = cJSON_CreateArray();
  cJSON *tempObj;
  for(int i=0; i<NR_OF_PORTS; i++){
    if(ports[i] == 255) {
      continue;
    }
    tempObj = cJSON_CreateObject();
    if( i>3 ){
      ev3_sensor_port_name(ports[i], port);
      cJSON_AddStringToObject(tempObj, "type", ev3_sensor_type( ev3_sensor[ ports[i] ].type_inx ));
    } else {
      ev3_tacho_port_name(ports[i], port);
      cJSON_AddStringToObject(tempObj, "type", ev3_tacho_type( ev3_tacho[ ports[i] ].type_inx ));
    }
    cJSON_AddStringToObject(tempObj, "port", port + strlen(port) -1);
    cJSON_AddItemToArray(root, tempObj);
  }
  *out_string = cJSON_Print(root);
  cJSON_Delete(root);
  return WRTCR_SUCCESS;
}

unsigned int get_port_index(char port){
  switch(port){
  case 'A':
    return 0;
    break;
  case 'B':
    return 1;
    break;
  case 'C':
    return 2;
    break;
  case 'D':
    return 3;
    break;
  case '1':
    return 4;
    break;
  case '2':
    return 5;
    break;
  case '3':
    return 6;
    break;
  case '4':
    return 7;
    break;
  default:
    return -1;
    break;
  }
}
