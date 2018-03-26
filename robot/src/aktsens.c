#include "aktsens.h"

static uint8_t sn_collision;
static uint8_t sn_sonar_zero;
static uint8_t sn_sonar;

wrtcr_rc setup_robot(){
  if( ev3_init() != 1){
    handle_err("Could not find EV3 brick", true);
  }
  int rc = ev3_sensor_init();
  if( rc == -1 ){
    handle_err("Could not find any senors", false);
  } else {
    ZF_LOGI("Found %d sensors", rc);
  }

  if(!ev3_search_sensor_plugged_in(INPUT_1, 0, &sn_collision, 0)){
    handle_err("Could not find sensor on port 1", false);
  } else {
    if( ev3_sensor_desc_type_inx(sn_collision) != LEGO_EV3_TOUCH){
      handle_err("Sensors attached to port 1 is not a touch sensor", false);
    }
  }

  if(!ev3_search_sensor_plugged_in(INPUT_2, 0, &sn_sonar_zero, 0)){
    handle_err("Could not find sensor on port 2", false);
  } else {
    if( ev3_sensor_desc_type_inx(sn_sonar_zero) != LEGO_EV3_TOUCH){
      handle_err("Sensors attached to port 2 is not a touch sensor", false);
    }
  }

  if(!ev3_search_sensor_plugged_in(INPUT_3, 0, &sn_sonar, 0)){
    handle_err("Could not find sensor on port 3", false);
  } else {
    if( ev3_sensor_desc_type_inx(sn_sonar) != LEGO_EV3_US){
      handle_err("Sensors attached to port 3 is not an ultrasound sensor", false);
    }
  }
  return WRTCR_SUCCESS;
}

wrtcr_rc check_collision() {
	int val;
	if(!get_sensor_value( 0, sn_collision, &val)){
    handle_err("Could not get value from collision sensor", false);
  }
  ZF_LOGI("%d\n", val);
  return WRTCR_SUCCESS;
}

wrtcr_rc get_distance(){
	float val;

	if(set_sensor_mode(sn_sonar, "US-SI-CM") && !get_sensor_value0( sn_sonar, &val)){
    handle_err("Could not get value from sonar sensor", false);
  }
  ZF_LOGI("%f\n", val);
  return WRTCR_SUCCESS;
}
