#include "aktsens.h"

static uint8_t sens_collision;
static uint8_t sens_sonar_zero;
static uint8_t sens_sonar;
static uint8_t mot_sonar;
static uint8_t mot_left;
static uint8_t mot_right;

static int sonar_pos;

wrtcr_rc init_sonar();

wrtcr_rc setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not find EV3 brick", true);
  }
  //intialize sensors
  int rc = ev3_sensor_init();
  if( rc == -1 ){
    handle_err("Could not find any senors", false);
  } else {
    ZF_LOGI("Found %d sensors", rc);
  }

  //intialize motors
  rc = ev3_tacho_init();
  if( rc == -1 ){
    handle_err("Could not find any motors", false);
  } else {
    ZF_LOGI("Found %d motors", rc);
  }

  //check for correct sensors
  if(!ev3_search_sensor_plugged_in(INPUT_1, 0, &sens_collision, 0)){
    handle_err("Could not find sensor on port 1", false);
  } else {
    if( ev3_sensor_desc_type_inx(sens_collision) != LEGO_EV3_TOUCH){
      handle_err("Sensors attached to port 1 is not a touch sensor", false);
    }
  }

  if(!ev3_search_sensor_plugged_in(INPUT_2, 0, &sens_sonar_zero, 0)){
    handle_err("Could not find sensor on port 2", false);
  } else {
    if( ev3_sensor_desc_type_inx(sens_sonar_zero) != LEGO_EV3_TOUCH){
      handle_err("Sensors attached to port 2 is not a touch sensor", false);
    }
  }

  if(!ev3_search_sensor_plugged_in(INPUT_3, 0, &sens_sonar, 0)){
    handle_err("Could not find sensor on port 3", false);
  } else {
    if( ev3_sensor_desc_type_inx(sens_sonar) != LEGO_EV3_US){
      handle_err("Sensors attached to port 3 is not an ultrasound sensor", false);
    }
  }

  //check for correct motors
  if(!ev3_search_tacho_plugged_in(OUTPUT_B, 0, &mot_sonar, 0)){
    handle_err("Could not find motor on port B", false);
  } else {
    if( get_tacho_type_inx(mot_sonar) != LEGO_EV3_M_MOTOR){
      handle_err("Motor attached to port B is not a medium tacho motor", false);
    }
  }

  if(!ev3_search_tacho_plugged_in(OUTPUT_C, 0, &mot_left, 0)){
    handle_err("Could not find motor on port C", false);
  } else {
    if( get_tacho_type_inx(mot_left) != LEGO_EV3_L_MOTOR){
      handle_err("Motor attached to port C is not a large tacho motor", false);
    }
  }

  if(!ev3_search_tacho_plugged_in(OUTPUT_D, 0, &mot_right, 0)){
    handle_err("Could not find motor on port C", false);
  } else {
    if( get_tacho_type_inx(mot_right) != LEGO_EV3_L_MOTOR){
      handle_err("Motor attached to port C is not a large tacho motor", false);
    }
  }
  
  init_sonar();
  return WRTCR_SUCCESS;
}

//move sonar to zero position and zero variables
wrtcr_rc init_sonar(){
  int val;
  do{
    if(!get_sensor_value( 0, sens_sonar_zero, &val)){
      handle_err("Could not get value from sonar zero sensor", false);
    }
    set_tacho_speed_sp( mot_sonar, 50);
    set_tacho_position_sp( mot_sonar, 5 );
    set_tacho_command_inx( mot_sonar, TACHO_RUN_TO_REL_POS );
  } while(val != 1);
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

wrtcr_rc get_distance(){
	float val;

	if(set_sensor_mode(sens_sonar, "US-SI-CM") && !get_sensor_value0(sens_sonar, &val)){
    handle_err("Could not get value from sonar sensor", false);
  }
  ZF_LOGI("%f\n", val);
  return WRTCR_SUCCESS;
}
