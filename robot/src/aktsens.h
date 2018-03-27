#pragma once

#include <ev3.h>
#include <ev3_sensor.h>
#include <ev3_tacho.h>
#include <ev3_port.h>

#include "utils.h"

wrtcr_rc setup_robot();

wrtcr_rc check_collision();

wrtcr_rc get_distance();
