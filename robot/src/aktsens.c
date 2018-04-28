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

static uint8_t sens_collision;
static uint8_t sens_sonar_zero;
static uint8_t sens_sonar;
static uint8_t mot_sonar;
static uint8_t mots_drive[3] = {0, 0, TACHO_DESC__LIMIT_};

static const float sonar_deg_factor = 180.0/((float)SONAR_ROT_LIMIT*2.0);

wrtcr_rc check_sensors();
wrtcr_rc check_motors();
wrtcr_rc init_sonar();
wrtcr_rc init_drive();

void* setup_robot(void *ignore){
  if(ignore != NULL){
    handle_err("Called setup robot with arguments", true);
  }
  if( ev3_init() != 1){
    handle_err("Could not initialize robot library", true);
  }

  check_sensors();
  check_motors();
  init_sonar();
  init_drive();

  return NULL;
}

