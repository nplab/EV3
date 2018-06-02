//feature test macro for nanosleep
#define _POSIX_C_SOURCE 199309L 

#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "metadevices.h"

//define meta device structures
typedef wrtcr_rc (*meta_device_function)(cJSON *msg);
typedef struct meta_device{
  meta_device_function handler;
  char *type;
} meta_device_t;
typedef map_t(meta_device_t) meta_device_map_t;

meta_device_map_t md_map;

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
  meta_device_t dev;
  dev.handler = handle_collision_sensor_message;
  dev.type = "meta-collision";
  map_set(&md_map, "a", dev);

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
    ZF_LOGI("Collision thread has ended.");
  } else {
    ZF_LOGE("Got message for collision sensor with unknown mode. Ignoring!");
    return WRTCR_FAILURE;
  }
  
  return WRTCR_SUCCESS;
}

void* collision_routine(void *ign){
  while(true){
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 10 *1000 * 1000;
    ZF_LOGI("Hey there!");
    nanosleep(&sleeptime, NULL);
  }
  return NULL;
}
