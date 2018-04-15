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

wrtcr_rc check_sensors(){
  //intialize sensors
  int result = ev3_sensor_init();
  if( result < 0 ){
    handle_err("Could not find any sensors", false);
    return WRTCR_FAILURE;
  } else {
    ZF_LOGI("Found %d sensors", result);
  }
  //check for correct sensors
  POSGE(ev3_search_sensor_plugged_in(INPUT_1, 0, &sens_collision, 0), "Could not find sensor on port 1", true);
  POSGE(ev3_sensor_desc_type_inx(sens_collision) == LEGO_EV3_TOUCH, "Sensor attached to port 1 is not a touch sensor", true);

  POSGE(ev3_search_sensor_plugged_in(INPUT_2, 0, &sens_sonar_zero, 0), "Could not find sensor on port 2", true);
  POSGE(ev3_sensor_desc_type_inx(sens_sonar_zero) == LEGO_EV3_TOUCH, "Sensor attached to port 2 is not a touch sensor", true);

  POSGE(ev3_search_sensor_plugged_in(INPUT_3, 0, &sens_sonar, 0), "Could not find sensor on port 3", true);
  POSGE(ev3_sensor_desc_type_inx(sens_sonar) == LEGO_EV3_US, "Sensor attached to port 3 is not an ultrasound sensor", true);
  POSGE(set_sensor_mode(sens_sonar, "US-DIST-CM"), "Could not initialize sonar sensor", true);

  return WRTCR_SUCCESS;
}

wrtcr_rc check_motors(){
  //intialize motors
  int result = ev3_tacho_init();
  if( result == -1 ){
    handle_err("Could not find any motors", false);
    return WRTCR_FAILURE;
  } else {
    ZF_LOGI("Found %d motors", result);
  }

  //check for correct motors
  POSGE(ev3_search_tacho_plugged_in(OUTPUT_A, 0, &mot_sonar, 0), "Could not find motor on port B", true);
  POSGE(get_tacho_type_inx(mot_sonar) == LEGO_EV3_M_MOTOR, "Motor attached to port B is not a medium tacho motor", true);

  POSGE(ev3_search_tacho_plugged_in(OUTPUT_C, 0, &MOT_LEFT, 0), "Could not find motor on port C", true);
  POSGE(get_tacho_type_inx(MOT_LEFT) == LEGO_EV3_L_MOTOR, "Motor attached to port C is not a large tacho motor", true);

  POSGE(ev3_search_tacho_plugged_in(OUTPUT_D, 0, &MOT_RIGHT, 0), "Could not find motor on port C", true);
  POSGE(get_tacho_type_inx(MOT_RIGHT) == LEGO_EV3_L_MOTOR, "Motor attached to port C is not a large tacho motor", true);

  int ms;
  get_tacho_max_speed(MOT_LEFT, &ms);
  ZF_LOGI("VMax = %d", ms);

  return WRTCR_SUCCESS;
}

//move sonar to zero position and zero variables
wrtcr_rc init_sonar(){
  int val;

  //reset motor, set speed for moving to zero and make it attempt to hold its position on stop
  POSGE(set_tacho_command_inx(mot_sonar, TACHO_RESET) && set_tacho_speed_sp(mot_sonar, -SONAR_MOT_SPEED) && set_tacho_stop_action_inx(mot_sonar, TACHO_HOLD), "Could not configure sonar motor", true);
  //run to zero position
  POSGE(set_tacho_command_inx(mot_sonar, TACHO_RUN_FOREVER), "Could not start moving towards zero on sonar motor", true);
  do{
    POSGE(get_sensor_value( 0, sens_sonar_zero, &val),"Could not get value from sonar zero sensor", false);
  } while(val != 1);
  POSGE(set_tacho_command_inx(mot_sonar, TACHO_STOP), "Could not stop sonar motor", true);
  sleep(1); //needs to be here, otherwise the set_position below is seen as a command to move for whatever reason
  //set current motor position as left stop
  POSGE(set_tacho_position(mot_sonar, -1*SONAR_ROT_LIMIT), "Could not set zero value for sonar motor", true);
  return WRTCR_SUCCESS;
}

wrtcr_rc init_drive(){
  //reset motors and let them rotate freely after a stop
  POSGE(multi_set_tacho_command_inx(mots_drive, TACHO_RESET) &&multi_set_tacho_stop_action_inx(mots_drive, TACHO_COAST), "Could not intialize drive motors", true);
  return WRTCR_SUCCESS;
}

wrtcr_rc check_collision() {
	int val;
	if(!get_sensor_value( 0, sens_collision, &val)){
    handle_err("Could not get value from collision sensor", false);
  }
  ZF_LOGI("%d\n", val);
  return WRTCR_SUCCESS;
}

wrtcr_rc get_distance(float *val){
  static int8_t direction = 1;
  uint8_t state;
	int left_limit, pos;


  //move and get value
  POSGE(set_tacho_position_sp( mot_sonar, direction*SONAR_STEP_DEG) && set_tacho_command_inx(mot_sonar, TACHO_RUN_TO_REL_POS), "Could not make sonar motor move", true);
  do{
    POSGE(get_tacho_state_flags(mot_sonar, &state), "Could not get state of sonar motor", false);
  }while( state == TACHO_RUNNING);

  //check limits
  POSGE(get_sensor_value( 0, sens_sonar_zero, &left_limit), "Could not get value from sonar zero sensor", true);
  POSGE(get_tacho_position(mot_sonar, &pos), "Could not get position value from sonar motor", true);
  if(left_limit == 1){ //if the sonar is at the left limit of its travel, set clockwise direction and zero position
    direction = 1;
    set_tacho_position(mot_sonar, -1*SONAR_ROT_LIMIT);;
  } else if(pos >= SONAR_ROT_LIMIT){ //if the sonar is at the right limit of its travel, set counter-clockwise direction
    direction = -1;
  }

	POSGE(get_sensor_value0(sens_sonar, val), "Could not get value from sonar sensor", false);
  return WRTCR_SUCCESS;
}

wrtcr_rc drive(int8_t *speeds){
  int speeds_normalized[2];
  speeds_normalized[0] = MOT_VMAX_DEG*speeds[0]/100;
  speeds_normalized[1] = MOT_VMAX_DEG*speeds[1]/100;
  POSGE(set_tacho_speed_sp(MOT_LEFT, speeds_normalized[0]) && set_tacho_speed_sp(MOT_RIGHT, speeds_normalized[1]), "Could not set speeds on drive motors", false);
  POSGE(multi_set_tacho_command_inx(mots_drive, TACHO_RUN_FOREVER), "Could not send run command to drive motors", false);
  return WRTCR_SUCCESS;
}
