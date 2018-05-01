#include "aktsens.h"

//print on error of the robot set or get functions which return the number of bytes written or read
#define POSGE(rc, msg, fail) if(!(rc)){handle_err(msg, false); if(fail) return WRTCR_FAILURE;}
#define NR_OF_PORTS 8
static psn* ports[NR_OF_PORTS] = {};

unsigned int get_port_index(char port);
wrtcr_rc add_port(const char port, const char *type, const uint8_t sn);

void* setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not initialize robot library", true);
  }
  if( ev3_sensor_init() < 1){
    handle_err("Could not initialize sensors", true);
  }
  /* char mode[ 256 ]; */
  char port[ 8 ];
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
    if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
      ev3_sensor_port_name( i, port );
      /* get_sensor_mode( i, mode, sizeof( mode )); */
      EOE(add_port(port[2], ev3_sensor_type( ev3_sensor[ i ].type_inx ), ev3_search_port(i, EXT_PORT__NONE_)), "Could not add found sensor to port array");
    }
  }
  if( ev3_tacho_init() < 1){
    handle_err("Could not initialize motors", true);
  }
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
      if ( ev3_tacho[ i ].type_inx != TACHO_TYPE__NONE_ ) {
      printf( "  type = %s\n", ev3_tacho_type( ev3_tacho[ i ].type_inx ));
      printf( "  port %d= %s\n", ev3_tacho[i].port, ev3_tacho_port_name( i, port ));
    }
  }
  return NULL;
}

wrtcr_rc add_port(const char port, const char *type, const uint8_t sn){
  int type_len = strlen(type)+1;
  psn* temp = (psn *)malloc(sizeof(psn));
  EON(temp, "Could not allocate memory for port structure");
  temp->sn = sn;
  temp->type = (char *)malloc(type_len);
  EON(temp->type, "Could not allocate memory for port type");
  strcpy(temp->type, type);
  ports[get_port_index(port)] = temp;
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
