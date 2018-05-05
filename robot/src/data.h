#pragma once

#include <rawrtc.h>
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
  struct data_channel_helper* data_channel_negotiated;
  struct data_channel_helper* data_channel;
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
