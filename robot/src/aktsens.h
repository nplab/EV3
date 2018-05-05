#pragma once

#include <ev3.h>
#include <ev3_sensor.h>
#include <ev3_tacho.h>
#include <ev3_port.h>
#include <cJSON.h>

#include "utils.h"

typedef struct port_serial_number {
  char *type;
  uint8_t sn;
} psn;

wrtcr_rc setup_robot();

wrtcr_rc get_port_description(char **out_string);
