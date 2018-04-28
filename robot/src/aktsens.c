#include "aktsens.h"

#define SONAR_ROT_LIMIT 220
#define SONAR_STEP_DEG 15
#define SONAR_MOT_SPEED 100
#define MOT_LEFT mots_drive[0]
#define MOT_RIGHT mots_drive[1]
#define MOT_VMAX_DEG 1050
#define MOT_VMIN_DEG 10

//print on error of the robot set or get functions which return the number of bytes written or read
#define POSGE(rc, msg, fail) if(!(rc)){handle_err(msg, false); if(fail) return WRTCR_FAILURE;}

static const float sonar_deg_factor = 180.0/((float)SONAR_ROT_LIMIT*2.0);

wrtcr_rc check_sensors();
wrtcr_rc check_motors();
wrtcr_rc init_sonar();
wrtcr_rc init_drive();

void* setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not initialize robot library", true);
  }
  if( ev3_sensor_init() < 1){
    handle_err("Could not initialize sensors", true);
  }
  char s[ 256 ];
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
    if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
      printf( "  type = %s\n", ev3_sensor_type( ev3_sensor[ i ].type_inx ));
      printf( "  port = %s\n", ev3_sensor_port_name( i, s ));
      if ( get_sensor_mode( i, s, sizeof( s ))) {
        printf( "  mode = %s\n", s );
      }
    }
  }
  if( ev3_tacho_init() < 1){
    handle_err("Could not initialize motors", true);
  }
  for ( int i = 0; i < DESC_LIMIT; i++ ) {
      if ( ev3_tacho[ i ].type_inx != TACHO_TYPE__NONE_ ) {
      printf( "  type = %s\n", ev3_tacho_type( ev3_tacho[ i ].type_inx ));
      printf( "  port = %s\n", ev3_tacho_port_name( i, s ));
    }
  }



  return NULL;
}

