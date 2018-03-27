#include "aktsens.h"

#define SONAR_RIGHT_LIMIT 220
#define SONAR_STEP_DEG 15
#define SONAR_MOT_SPEED 100
#define MOT_LEFT mots_drive[0]
#define MOT_RIGHT mots_drive[1]

static uint8_t sens_collision;
static uint8_t sens_sonar_zero;
static uint8_t sens_sonar;
static uint8_t mot_sonar;
static uint8_t mots_drive[2];

static const float sonar_deg_factor = 180.0/((float)SONAR_RIGHT_LIMIT*2.0);

wrtcr_rc init_sonar();
wrtcr_rc check_sensors();
wrtcr_rc check_motors();

wrtcr_rc setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not find EV3 brick", true);
  }

  check_sensors();
  check_motors();
  init_sonar();
  ZF_LOGI("%f", sonar_deg_factor);
  return WRTCR_SUCCESS;
}

wrtcr_rc check_sensors(){
  wrtcr_rc rc = WRTCR_SUCCESS;

  //intialize sensors
  int result = ev3_sensor_init();
  if( result == -1 ){
    handle_err("Could not find any sensors", false);
    return WRTCR_FAILURE;
  } else {
    ZF_LOGI("Found %d sensors", result);
  }
  //check for correct sensors
  if(!ev3_search_sensor_plugged_in(INPUT_1, 0, &sens_collision, 0)){
    handle_err("Could not find sensor on port 1", false);
    rc = WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(sens_collision) != LEGO_EV3_TOUCH){
      handle_err("Sensors attached to port 1 is not a touch sensor", false);
      rc = WRTCR_FAILURE;
    }
  }

  if(!ev3_search_sensor_plugged_in(INPUT_2, 0, &sens_sonar_zero, 0)){
    handle_err("Could not find sensor on port 2", false);
    rc = WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(sens_sonar_zero) != LEGO_EV3_TOUCH){
      handle_err("Sensors attached to port 2 is not a touch sensor", false);
      rc = WRTCR_FAILURE;
    }
  }

  if(!ev3_search_sensor_plugged_in(INPUT_3, 0, &sens_sonar, 0)){
    handle_err("Could not find sensor on port 3", false);
    rc = WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(sens_sonar) != LEGO_EV3_US){
      handle_err("Sensors attached to port 3 is not an ultrasound sensor", false);
      rc = WRTCR_FAILURE;
    }else{
      if(set_sensor_mode(sens_sonar, "US-DIST-CM")){
        handle_err("Could not initialize sonar sensor", false);
        rc = WRTCR_FAILURE;
      }
    }
  }

  return rc;
}

wrtcr_rc check_motors(){
  wrtcr_rc rc = WRTCR_SUCCESS;
  //intialize motors
  int result = ev3_tacho_init();
  if( result == -1 ){
    handle_err("Could not find any motors", false);
    return WRTCR_FAILURE;
  } else {
    ZF_LOGI("Found %d motors", result);
  }

  //check for correct motors
  if(!ev3_search_tacho_plugged_in(OUTPUT_A, 0, &mot_sonar, 0)){
    handle_err("Could not find motor on port B", false);
    rc = WRTCR_FAILURE;
  } else {
    if( get_tacho_type_inx(mot_sonar) != LEGO_EV3_M_MOTOR){
      handle_err("Motor attached to port B is not a medium tacho motor", false);
      rc = WRTCR_FAILURE;
    }
  }

  if(!ev3_search_tacho_plugged_in(OUTPUT_C, 0, &MOT_LEFT, 0)){
    handle_err("Could not find motor on port C", false);
    rc = WRTCR_FAILURE;
  } else {
    if( get_tacho_type_inx(MOT_LEFT) != LEGO_EV3_L_MOTOR){
      handle_err("Motor attached to port C is not a large tacho motor", false);
      rc = WRTCR_FAILURE;
    }
  }

  if(!ev3_search_tacho_plugged_in(OUTPUT_D, 0, &MOT_RIGHT, 0)){
    handle_err("Could not find motor on port C", false);
    rc = WRTCR_FAILURE;
  } else {
    if( get_tacho_type_inx(MOT_RIGHT) != LEGO_EV3_L_MOTOR){
      handle_err("Motor attached to port C is not a large tacho motor", false);
      rc = WRTCR_FAILURE;
    }
  }
  return rc;
}

//move sonar to zero position and zero variables
wrtcr_rc init_sonar(){
  int val;
  int max_speed;

  set_tacho_command_inx(mot_sonar, TACHO_RESET);
  get_tacho_max_speed( mot_sonar, &max_speed);
  set_tacho_speed_sp(mot_sonar, -SONAR_MOT_SPEED);//make the sonar turn counter-clockwise at a tenth of the maximum speed
	set_tacho_stop_action_inx(mot_sonar, TACHO_HOLD); //make the motor attempt to hold its stop position
  set_tacho_command_inx(mot_sonar, TACHO_RUN_FOREVER);
  do{
    if(!get_sensor_value( 0, sens_sonar_zero, &val)){
      handle_err("Could not get value from sonar zero sensor", false);
    }
  } while(val != 1);
  set_tacho_command_inx(mot_sonar, TACHO_STOP);
  set_tacho_position(mot_sonar, -1*SONAR_RIGHT_LIMIT);
  return WRTCR_SUCCESS;
}

wrtcr_rc init_drive(){

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
  wrtcr_rc rc = WRTCR_SUCCESS;
  static int8_t direction = 1;
  uint8_t state;
	int left_limit, pos;


  //move and get value
  set_tacho_position_sp( mot_sonar, direction*SONAR_STEP_DEG);
  set_tacho_command_inx(mot_sonar, TACHO_RUN_TO_REL_POS);
  do{
    get_tacho_state_flags(mot_sonar, &state);
  }while( state == TACHO_RUNNING);

  //check limits
  if(!get_sensor_value( 0, sens_sonar_zero, &left_limit)){
    handle_err("Could not get value from sonar zero sensor", true);
    rc = WRTCR_FAILURE;
  }
  if(!get_tacho_position(mot_sonar, &pos)){
    handle_err("Could not get position value from sonar motor", true);
    rc = WRTCR_FAILURE;
  }
  if(left_limit == 1){ //if the sonar is at the left limit of its travel, move clockwise
    direction = 1;
    set_tacho_position(mot_sonar, -1*SONAR_RIGHT_LIMIT);
  }

  if(pos >= SONAR_RIGHT_LIMIT){ //if the sonar is at the right limit of its travel, move counter-clockwise
    direction = -1;
  }

	if(!get_sensor_value0(sens_sonar, val)){
    handle_err("Could not get value from sonar sensor", false);
    rc = WRTCR_FAILURE;
  }
  ZF_LOGI("%f at %d°\n", *val, (int)(pos*sonar_deg_factor));
  fflush(stdout);
  return rc;
}

wrtcr_rc drive(){
  
}
  
