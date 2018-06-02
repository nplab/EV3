#include "metadevices.h"

//define meta device port map
typedef wrtcr_rc (*meta_device_function)(cJSON *msg);
typedef map_t(meta_device_function) meta_device_map_t;

wrtcr_rc handle_collision_sensor_message(cJSON *msg);

wrtcr_rc setup_meta_devices(){

  
  return WRTCR_SUCCESS;
}

wrtcr_rc get_meta_device_description(cJSON **meta_devices){
  cJSON *root = cJSON_CreateArray();
  cJSON *dev = cJSON_CreateObject();

  //add meta devices to desription array hard coded
  cJSON_AddStringToObject(dev, "port", "a");
  cJSON_AddStringToObject(dev, "type", "meta-collision");
  cJSON_AddItemToArray(root, dev);

  *meta_devices = root;
  return WRTCR_SUCCESS;
}

wrtcr_rc handle_meta_device_message(char *port, cJSON *message){

  return WRTCR_SUCCESS;
}

wrtcr_rc handle_collision_sensor_message(cJSON *msg){
  
  return WRTCR_SUCCESS;
}
