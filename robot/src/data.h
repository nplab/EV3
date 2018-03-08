#pragma once

#include <rawrtc.h>
#include "config.h"
#include "utils.h"

//client side meta info
struct peer_connection_client {
  char* name;
  bool offering;
  struct rawrtc_peer_connection_configuration* configuration;
  struct data_channel_helper* data_channel_negotiated;
  struct data_channel_helper* data_channel;
};

//set up the data channel to the website
wrtcr_rc data_channel_setup();

//shut down data channel and clean up
wrtcr_rc data_channel_shutdown();
