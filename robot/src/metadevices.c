//feature test macro for nanosleep
#define _POSIX_C_SOURCE 199309L 

#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "metadevices.h"

//error handling macro
#define POSGE(rc, msg, fail) if(!(rc)){handle_err(msg, false); if(fail) return NULL;}

//define meta device structures
typedef wrtcr_rc (*meta_device_function)(cJSON *msg);
typedef struct meta_device{
  meta_device_function handler;
  char *type;
} meta_device_t;
typedef map_t(meta_device_t) meta_device_map_t;
meta_device_map_t md_map;

//define meta device ports
char *collision_port = "a";
char *sonar_port = "b";

//save necessary serial numbers
uint8_t collision_sn;
uint8_t us_sn;
uint8_t zero_sn;
uint8_t sonar_motor_sn;

//define non-public functions
// -- collision sensors
wrtcr_rc setup_collision_sensor();
wrtcr_rc handle_collision_sensor_message(cJSON *msg);
void* collision_routine(void *ign);
// -- sonar sensor
wrtcr_rc setup_sonar_sensor();
wrtcr_rc handle_sonar_sensor_message(cJSON *msg);
void* sonar_routine(void *ign);

wrtcr_rc setup_meta_devices(){
  map_init(&md_map);
  
  EOE(setup_collision_sensor(), "Could not set up collision sensor. Quitting!");
  EOE(setup_sonar_sensor(), "Could not set up sonar sensor. Quitting!");

  return WRTCR_SUCCESS;
}

wrtcr_rc cleanup_meta_devices(){
  map_deinit(&md_map);
  return WRTCR_SUCCESS;
}

wrtcr_rc get_meta_device_description(cJSON **meta_devices){
  cJSON *root = cJSON_CreateArray();
  cJSON *dev;

  //iterate over map to add meta devices to json array
  map_iter_t iter = map_iter(&md_map);
  const char *portkey; //possibly the triwizard cup... or just an old boot
  while ((portkey = map_next(&md_map, &iter))) {
    dev = cJSON_CreateObject();
    cJSON_AddStringToObject(dev, "port", portkey);
    cJSON_AddStringToObject(dev, "type", map_get(&md_map, portkey)->type);
    cJSON_AddItemToArray(root, dev);
  }

  *meta_devices = root;
  return WRTCR_SUCCESS;
}

wrtcr_rc handle_meta_device_message(char *port, cJSON *message){
  cJSON *mode = cJSON_GetObjectItem(message, "mode");
  if(mode == NULL || !cJSON_IsString(mode)){
    ZF_LOGE("Got message for meta device without mode. Ignoring!");
    return WRTCR_FAILURE;
  }
  return map_get(&md_map, port)->handler(message);
}

//--------- collision sensor functions --------------------//
wrtcr_rc setup_collision_sensor(){
  //check for sensor
  if(!ev3_search_sensor_plugged_in(INPUT_1, 0, &collision_sn, 0)){
    ZF_LOGE("Could not find sensor on port 1");
    return WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(collision_sn) != LEGO_EV3_TOUCH){
      ZF_LOGE("Sensors attached to port 1 is not a touch sensor");
      return WRTCR_FAILURE;
    }
  }

  meta_device_t dev;
  dev.handler = handle_collision_sensor_message;
  dev.type = "meta-collision";
  map_set(&md_map, collision_port, dev);

  return WRTCR_SUCCESS;
}

wrtcr_rc handle_collision_sensor_message(cJSON *msg){
  static pthread_t collision_thread_id;
  static bool thread_exists = false;
  cJSON *mode = cJSON_GetObjectItem(msg, "mode");

  if(strncmp("start", mode->valuestring, strlen("start")) == 0){
    if(thread_exists){
      ZF_LOGW("Got message requiring start of collision detection thread, but it is already running");
      return WRTCR_FAILURE;
    }
    if(pthread_create(&collision_thread_id, NULL, &collision_routine, NULL) == 0){
      thread_exists = true;
      ZF_LOGI("Created collision detection thread.");
    } else {
      ZF_LOGE("Could not create collision detection thread.");
      return WRTCR_FAILURE;
    }
  } else if(strncmp("stop", mode->valuestring, strlen("stop")) == 0){
    if(pthread_cancel(collision_thread_id) != 0){
      ZF_LOGE( "Could not send cancellation request to collision detection thread.");
      return WRTCR_FAILURE;
    }
    pthread_join(collision_thread_id, NULL);
    thread_exists = false;
    ZF_LOGI("Collision detection thread has ended.");
  } else {
    ZF_LOGE("Got message for collision sensor with unknown mode. Ignoring!");
    return WRTCR_FAILURE;
  }
  
  return WRTCR_SUCCESS;
}

void* collision_routine(void *ign){
  static meta_device_t* sensor = NULL;
  static int last_value = 0;
  static struct timespec sleeptime;
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 500 * 1000 * 1000;

  //get sensor if necessary
  if(!sensor){
    sensor = map_get(&md_map, collision_port);
    if(sensor == NULL){
      handle_err("Could not find collision sensor structure. Should never happen! Exiting!", true);
    }
  }

  int cur_value;
  char msg[30];

  while(true){
    if(!get_sensor_value( 0, collision_sn, &cur_value)){
      ZF_LOGE("Could not get value from collision sensor");
    } else if( cur_value != last_value ){
      snprintf(msg, sizeof(msg), "{\"port\": \"%s\", \"value\":[%d]}", collision_port, cur_value);
      POE(send_message_on_sensor_channel(msg), "Could not send collision sensor message");
      last_value = cur_value;
    }
    nanosleep(&sleeptime, NULL);
  }
  return NULL;
}

//--------- sonar sensor functions --------------------//
wrtcr_rc setup_sonar_sensor(){
  //check for zero sensor
  if(!ev3_search_sensor_plugged_in(INPUT_2, 0, &zero_sn, 0)){
    ZF_LOGE("Could not find sensor on port 2");
    return WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(zero_sn) != LEGO_EV3_TOUCH){
      ZF_LOGE("Sensor attached to port 2 is not a touch sensor");
      return WRTCR_FAILURE;
    }
  }
  //check for ultrasonic sensor
  if(!ev3_search_sensor_plugged_in(INPUT_3, 0, &us_sn, 0)){
    ZF_LOGE("Could not find sensor on port 3");
    return WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(us_sn) != LEGO_EV3_US){
      ZF_LOGE("Sensor attached to port 3 is not an ultrasonic sensor");
      return WRTCR_FAILURE;
    }
  }
  //check for motomotor
  if(!ev3_search_tacho_plugged_in(OUTPUT_A, 0, &sonar_motor_sn, 0)){
    ZF_LOGE("Could not find motor on port A");
    return WRTCR_FAILURE;
  } else {
    if( ev3_tacho_desc_type_inx(sonar_motor_sn) !=  LEGO_EV3_M_MOTOR){
      ZF_LOGE("Motor attached to port A is not a lego m motor.");
      return WRTCR_FAILURE;
    }
  }

  meta_device_t dev;
  dev.handler = handle_sonar_sensor_message;
  dev.type = "meta-sonar";
  map_set(&md_map, sonar_port, dev);

  return WRTCR_SUCCESS;

}

wrtcr_rc handle_sonar_sensor_message(cJSON *msg){
  static pthread_t sonar_thread_id;
  static bool thread_exists = false;
  cJSON *mode = cJSON_GetObjectItem(msg, "mode");

  if(strncmp("start", mode->valuestring, strlen("start")) == 0){
    if(thread_exists){
      ZF_LOGW("Got message requiring start of collision detection thread, but it is already running");
      return WRTCR_FAILURE;
    }
    if(pthread_create(&sonar_thread_id, NULL, &sonar_routine, NULL) == 0){
      thread_exists = true;
      ZF_LOGI("Created sonar sensor thread.");
    } else {
      ZF_LOGE("Could not create sonar sensor thread.");
      return WRTCR_FAILURE;
    }
  } else if(strncmp("stop", mode->valuestring, strlen("stop")) == 0){
    if(pthread_cancel(sonar_thread_id) != 0){
      ZF_LOGE( "Could not send cancellation request to sonar sensor thread.");
      return WRTCR_FAILURE;
    }
    pthread_join(sonar_thread_id, NULL);
    thread_exists = false;
    ZF_LOGI("Sonar sensor thread has ended.");
  } else {
    ZF_LOGE("Got message for sonar sensor with unknown mode. Ignoring!");
    return WRTCR_FAILURE;
  }
  
  return WRTCR_SUCCESS;
  
}

void* sonar_routine(void *ign){
  static const int sonar_rot_limit = 220; //sonar motor position at left and right stop, in motor degrees
  static const int step_degree = 15; //angle to turn between measurements, in motor degrees
  static const int sonar_speed = 1200; //speed with which to turn the sonar, in degree per Minute (max is 1560)
  const float sonar_deg_factor = 180.0/((float)sonar_rot_limit*2.0);
  static struct timespec sleeptime;
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 500 * 1000 * 1000;

  { //set up the sonar sensors
    int val = 0;
    //reset motor, set speed for moving to zero and make it attempt to hold its position on stop
    POSGE(set_tacho_command_inx(sonar_motor_sn, TACHO_RESET) && set_tacho_speed_sp(sonar_motor_sn, -1*sonar_speed*0.15) && set_tacho_stop_action_inx(sonar_motor_sn, TACHO_HOLD), "Could not configure sonar motor", true);
    //run to zero position
    POSGE(set_tacho_command_inx(sonar_motor_sn, TACHO_RUN_FOREVER), "Could not start moving towards zero on sonar motor", true);
    do{
      get_sensor_value( 0, zero_sn, &val); //no error checks here, to speed things up
    } while(val != 1);
    POSGE(set_tacho_command_inx(sonar_motor_sn, TACHO_STOP), "Could not stop sonar motor", true);
    //set current motor position as left stop
    POSGE(set_tacho_position(sonar_motor_sn, -1*sonar_rot_limit), "Could not set zero value for sonar motor", true);
    POSGE(set_sensor_mode(us_sn, "US-DIST-CM"), "Could not initialize sonar sensor", true);
    POSGE(set_tacho_speed_sp(sonar_motor_sn, sonar_speed), "Could not set final run speed of sonar motor", false);
  }

  int8_t direction = 1;
  uint8_t state;
  int left_limit, pos;
  char msg[35];
  float value;

  while(true){
    do{
      POSGE(get_tacho_state_flags(sonar_motor_sn, &state), "Could not get state of sonar motor", false);
    } while( state == TACHO_RUNNING );

    //check limits
    POSGE(get_sensor_value( 0, zero_sn, &left_limit), "Could not get value from sonar zero sensor", true);
    POSGE(get_tacho_position(sonar_motor_sn, &pos), "Could not get position value from sonar motor", true);
    if(left_limit == 1){ //if the sonar is at the left limit of its travel, set clockwise direction and zero position
      direction = 1;
      set_tacho_position(sonar_motor_sn, -1*sonar_rot_limit);
    } else if(pos >= sonar_rot_limit){ //if the sonar is at the right limit of its travel, set counter-clockwise direction
      direction = -1;
    }

    if(get_sensor_value0(us_sn, &value) < 1){
      ZF_LOGE("Could not get value from sonar sensor");
      continue;
    }
    snprintf(msg, sizeof(msg), "{\"port\": \"%s\", \"value\":[%d, %d]}", sonar_port, (int)(pos*sonar_deg_factor), (int)value);
    POE(send_message_on_sensor_channel(msg), "Could not send sonar sensor message");

    //move and get value
    POSGE(set_tacho_position_sp( sonar_motor_sn, direction*step_degree) && set_tacho_command_inx(sonar_motor_sn, TACHO_RUN_TO_REL_POS), "Could not make sonar motor move", true);
    nanosleep(&sleeptime, NULL);
  }
}
