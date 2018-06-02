#pragma once

#include <ev3.h>
#include <ev3_sensor.h>
#include <ev3_tacho.h>
#include <ev3_port.h>
#include <cJSON.h>

#include "utils.h"
#include "data.h"

wrtcr_rc get_meta_device_description(cJSON **meta_devices);
