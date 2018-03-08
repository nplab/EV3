#include "data.h"

static struct peer_connection_client client_info = {0};
static struct rawrtc_peer_connection_configuration *configuration;

//initialisation and shutdown functions
wrtcr_rc initialise_client();
wrtcr_rc client_stop();

//signal handlers
static void negotiation_needed_handler(void* const arg);
static void connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg);
static void data_channel_open_handler(void* const arg);

wrtcr_rc data_channel_setup(){
  unsigned int stun_urls_length;
  char **stun_urls;
  unsigned int turn_urls_length;
  char **turn_urls;

  //get servers from config
  conf_get_string_array("stun_urls", &stun_urls, &stun_urls_length);
  conf_get_string_array("turn_urls", &turn_urls, &turn_urls_length);

  //initialize RawRTC
  if( rawrtc_init() != RAWRTC_CODE_SUCCESS ){
    handle_err("RawRTC Initialisaton failed", true);
  }

  //set up configuration
  if( rawrtc_peer_connection_configuration_create(&configuration, RAWRTC_ICE_GATHER_POLICY_ALL) != RAWRTC_CODE_SUCCESS ){
    handle_err("Could not create RawRTC configuration", true);
  }

  if( rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, stun_urls, stun_urls_length,
                                                          NULL, NULL, RAWRTC_ICE_CREDENTIAL_TYPE_NONE) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not add STUN servers to RawRTC configuration", true);
  }

  if( rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, turn_urls, turn_urls_length,
                                                          "threema-angular", "Uv0LcCq3kyx6EiRwQW5jVigkhzbp70CjN2CJqzmRxG3UGIdJHSJV6tpo7Gj7YnGB",
                                                          RAWRTC_ICE_CREDENTIAL_TYPE_PASSWORD) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not add TURN servers to  RawRTC configuration", true);
  }

  //set up client information
  client_info.name = "WebRTCRobot";
  client.offering = true;

  initialise_client();

  return WRTCR_SUCCESS;
}

wrtcr_rc data_channel_shutdown(){
  client_stop();
  rawrtc_close();
  return WRTCR_SUCCESS;
}

wrtcr_rc initialise_client(){
  struct rawrtc_data_channel_parameters* dc_parameters;
  //create peer connection
  if( rawrtc_peer_connection_create(
                                    &client_info.connection, configuration,
                                    negotiation_needed_handler, default_peer_connection_local_candidate_handler,
                                    default_peer_connection_local_candidate_error_handler,
                                    default_signaling_state_change_handler, default_ice_transport_state_change_handler,
                                    default_ice_gatherer_state_change_handler, connection_state_change_handler,
                                    default_data_channel_handler, client) != RAWRTC_CODE_SUCCESS ){
    handle_err("Could not create peer connection", true);
  }

  //create parameters for data channel
  data_channel_helper_create(&client_info->data_channel_negotiated, (struct client *) client_info, "wrtc_robot");

  if(rawrtc_data_channel_parameters_create(
                                           &dc_parameters, client_info.data_channel_negotiated->label,
                                           RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, true, 0) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not create data channel parameters", true);
  }

  //create actual data channel
  if(rawrtc_peer_connection_create_data_channel(
                                                &client_info.data_channel_negotiated->channel, client_info.connection,
                                                dc_parameters, NULL,
                                                default_data_channel_open_handler, default_data_channel_buffered_amount_low_handler,
                                                default_data_channel_error_handler, default_data_channel_close_handler,
                                                default_data_channel_message_handler, client_info->data_channel_negotiated) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not create data channel", true);
  }

  //clean up
  mem_deref(dc_parameters);
  return WRTCR_SUCCESS;
}

wrtcr_rc client_stop(){
  wrtcr_rc rc = WRTCR_SUCCESS;
  if(rawrtc_peer_connection_close(client_info.connection) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not close data channel", false);
    rc = WRTCR_FAILURE;
  }
  client_info->data_channel = mem_deref(client_info.data_channel);
  client_info->data_channel_negotiated = mem_deref(client_info.data_channel_negotiated);
  client_info->connection = mem_deref(client_info.connection);
  client_info->configuration = mem_deref(client_info.configuration);

  return rc;
}

static void negotiation_needed_handler(void* const arg) {
  struct peer_connection_client* const client = arg;

  // Print negotiation needed
  default_negotiation_needed_handler(arg);

  // Offering: Create and set local description
  if (client->offering) {
    struct rawrtc_peer_connection_description* description;
    if(rawrtc_peer_connection_create_offer(&description, client->connection, false) != RAWRTC_CODE_SUCCESS){
      handle_err("Could not create offer", true);
    }
    if(rawrtc_peer_connection_set_local_description(client->connection, description) != RAWRTC_CODE_SUCCES){
      handle_err("Could not create offer", true);
    }
    mem_deref(description);
  }
}

static void connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg) {
    struct peer_connection_client* const client = arg;

    // Print state
    default_peer_connection_state_change_handler(state, arg);

    // Only create a new channel if none exist yet (in case we disconnect and then reconnect)
    if (!client->data_channel && state == RAWRTC_PEER_CONNECTION_STATE_CONNECTED) {
        struct rawrtc_data_channel_parameters* channel_parameters;
        char* const label = client->offering ? "bear-noises" : "lion-noises";

        // Create data channel helper for in-band negotiated data channel
        data_channel_helper_create(
                &client->data_channel, (struct client *) client, label);

        // Create data channel parameters
        if(rawrtc_data_channel_parameters_create(&dc_parameters, client->data_channel->label, RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, false, 0) != RAWRTC_CODE_SUCCESS){
          handle_err("Could not create data channel parameters", true);
        }

        // Create data channel
        if(rawrtc_peer_connection_create_data_channel(&client->data_channel->channel, client->connection, dc_parameters, NULL, default_data_channel_open_handler, default_data_channel_buffered_amount_low_handler, default_data_channel_error_handler, default_data_channel_close_handler, default_data_channel_message_handler, client->data_channel) != RAWRTC_CODE_SUCCESS){
          handle_err("Could not create data channel", true);
        }

        // Un-reference data channel parameters
        mem_deref(dc_parameters);
    }
}
