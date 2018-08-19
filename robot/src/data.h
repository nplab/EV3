#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <rawrtc.h>
#pragma GCC diagnostic pop
#include <stdbool.h>
#include "signaling.h"
#include "default_handlers.h"
#include "config.h"
#include "utils.h"
#include "aktsens.h"

//client side meta info
struct client {
  char* name;
  char** ice_candidate_types;
  size_t n_ice_candidate_types;
  bool offering;
  struct rawrtc_peer_connection_configuration* configuration;
  struct rawrtc_peer_connection* connection;
  struct data_channel_helper* data_channel_api;
  struct data_channel_helper* data_channel_sensors;
  struct data_channel_helper* data_channel_ping;
};

//Data channel helper structure
struct data_channel_helper {
  struct le le;
  struct rawrtc_data_channel* channel;
  char* label;
  struct client* client;
  void* arg;
};

//set up the data channel to the website
wrtcr_rc data_channel_setup();

//shut down data channel and clean up
wrtcr_rc data_channel_shutdown();

//send message on ping data channel
wrtcr_rc send_message_on_ping_channel(char *msg);

//send string msg on the api channel
wrtcr_rc send_message_on_api_channel(char *msg);

//send msg with given port and value(s) on the sensor channel
wrtcr_rc send_message_on_sensor_channel(char *port, int value1);
wrtcr_rc send_long_message_on_sensor_channel(char *port, int value1, int value2);

//print an ice candidate
void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client);
