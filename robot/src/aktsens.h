#pragma once

#include <ev3.h>
#include <ev3_sensor.h>
#include <ev3_tacho.h>
#include <ev3_port.h>

#include "utils.h"

typedef struct port_serial_number {
  char *type;
  uint8_t sn;
} psn;

void* setup_robot();
