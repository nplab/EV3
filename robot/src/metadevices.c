//feature test macro for nanosleep
#define _POSIX_C_SOURCE 199309L 

#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "metadevices.h"

//define meta device ports
#define COLLISION_PORT "a"

//define meta device structures
typedef wrtcr_rc (*meta_device_function)(cJSON *msg);
typedef struct meta_device{
  meta_device_function handler;
  char *type;
  uint8_t sn;
} meta_device_t;
typedef map_t(meta_device_t) meta_device_map_t;
meta_device_map_t md_map;

//define non-public function
wrtcr_rc setup_collision_sensor();
wrtcr_rc handle_collision_sensor_message(cJSON *msg);
void* collision_routine(void *ign);

wrtcr_rc setup_meta_devices(){
  map_init(&md_map);
  
  EOE(setup_collision_sensor(), "Could not set up collision sensor. Quitting!");

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
  return map_get(&md_map, port)->handler(message);
}

wrtcr_rc setup_collision_sensor(){
  uint8_t sn;
  //check for sensor
  if(!ev3_search_sensor_plugged_in(INPUT_1, 0, &sn, 0)){
    ZF_LOGE("Could not find sensor on port 1");
    return WRTCR_FAILURE;
  } else {
    if( ev3_sensor_desc_type_inx(sn) != LEGO_EV3_TOUCH){
      ZF_LOGE("Sensors attached to port 1 is not a touch sensor");
      return WRTCR_FAILURE;
    }
  }

  meta_device_t dev;
  dev.handler = handle_collision_sensor_message;
  dev.type = "meta-collision";
  dev.sn = sn;
  map_set(&md_map, COLLISION_PORT, dev);

  return WRTCR_SUCCESS;
}

wrtcr_rc handle_collision_sensor_message(cJSON *msg){
  static pthread_t collision_thread_id;
  static bool thread_exists = false;

  cJSON *mode = cJSON_GetObjectItem(msg, "mode");
  if(mode == NULL || !cJSON_IsString(mode)){
    ZF_LOGE("Got message for collision sensor without mode. Ignoring!");
    return WRTCR_SUCCESS;
  }
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
    sensor = map_get(&md_map, COLLISION_PORT);
    if(sensor == NULL){
      handle_err("Could not find collision sensor structure. Should never happen! Exiting!", true);
    }
  }

  int cur_value;

  while(true){
    if(!get_sensor_value( 0, sensor->sn, &cur_value)){
      ZF_LOGE("Could not get value from collision sensor");
    } else if( cur_value != last_value ){
      char msg[30];
      snprintf(msg, sizeof(msg), "{\"port\": \"%s\", \"value\":[%d]}", COLLISION_PORT, cur_value);
      POE(send_message_on_sensor_channel(msg), "Could not send collision sensor message");
      last_value = cur_value;
    }
    nanosleep(&sleeptime, NULL);
  }
  return NULL;
}
