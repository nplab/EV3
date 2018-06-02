#include "metadevices.h"

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
