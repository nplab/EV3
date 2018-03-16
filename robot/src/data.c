#include "data.h"

static struct client client_info = {0};
static struct rawrtc_peer_connection_configuration *configuration;

//initialisation and shutdown functions
wrtcr_rc initialise_client();
wrtcr_rc client_stop();

//signal handlers
static void negotiation_needed_handler(void* const arg);
static void connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg);
void stop_on_return_handler(int flags, void* arg);
void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client);
void data_channel_helper_create(struct data_channel_helper** const channel_helperp, struct client* const client, char* const label);
static void data_channel_helper_destroy(void* arg);
bool ice_candidate_type_enabled(struct client* const client, enum rawrtc_ice_candidate_type const type);
static void parse_remote_description(int flags, void* arg);

wrtcr_rc data_channel_setup(){
  unsigned int stun_urls_length;
  char **stun_urls;
  unsigned int turn_urls_length;
  char **turn_urls;

  //get servers from config
  conf_get_string_array("stun_urls", &stun_urls, &stun_urls_length);
  conf_get_string_array("turn_urls", &turn_urls, &turn_urls_length);

  //initialize RawRTC
  EORE(rawrtc_init(),"RawRTC Initialisaton failed");

  //set up configuration
  EORE(rawrtc_peer_connection_configuration_create(&configuration, RAWRTC_ICE_GATHER_POLICY_ALL), "Could not create RawRTC configuration");

  EORE(rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, stun_urls, stun_urls_length,
                                                          NULL, NULL, RAWRTC_ICE_CREDENTIAL_TYPE_NONE), "Could not add STUN servers to RawRTC configuration");

  EORE(rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, turn_urls, turn_urls_length,
                                                          "threema-angular", "Uv0LcCq3kyx6EiRwQW5jVigkhzbp70CjN2CJqzmRxG3UGIdJHSJV6tpo7Gj7YnGB",
                                                          RAWRTC_ICE_CREDENTIAL_TYPE_PASSWORD), "Could not add TURN servers to  RawRTC configuration");

  //set up client information
  client_info.name = "WebRTCRobot";
  client_info.ice_candidate_types = NULL;
  client_info.n_ice_candidate_types = 0;
  client_info.configuration = configuration;
  client_info.offering = true;

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
  EORE(rawrtc_peer_connection_create(
                                    &client_info.connection, configuration,
                                    negotiation_needed_handler, default_peer_connection_local_candidate_handler,
                                    default_peer_connection_local_candidate_error_handler,
                                    default_signaling_state_change_handler, default_ice_transport_state_change_handler,
                                    default_ice_gatherer_state_change_handler, connection_state_change_handler,
                                    default_data_channel_handler, &client_info), "Could not create peer connection");

  //create parameters for data channel
  data_channel_helper_create(&client_info.data_channel_negotiated,  &client_info, "wrtc_robot");

  EORE(rawrtc_data_channel_parameters_create(
                                           &dc_parameters, client_info.data_channel_negotiated->label,
                                           RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, true, 0), "Could not create data channel parameters");

  //create actual data channel
  EORE(rawrtc_peer_connection_create_data_channel(
                                                &client_info.data_channel_negotiated->channel, client_info.connection,
                                                dc_parameters, NULL,
                                                default_data_channel_open_handler, default_data_channel_buffered_amount_low_handler,
                                                default_data_channel_error_handler, default_data_channel_close_handler,
                                                default_data_channel_message_handler, client_info.data_channel_negotiated), "Could not create data channel");

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
  client_info.data_channel = mem_deref(client_info.data_channel);
  client_info.data_channel_negotiated = mem_deref(client_info.data_channel_negotiated);
  client_info.connection = mem_deref(client_info.connection);
  client_info.configuration = mem_deref(client_info.configuration);

  return rc;
}

static void negotiation_needed_handler(void* const arg) {
  struct client* const client = arg;

  // Print negotiation needed
  default_negotiation_needed_handler(arg);

  // Offering: Create and set local description
  if (client->offering) {
    struct rawrtc_peer_connection_description* description;
    EORE(rawrtc_peer_connection_create_offer(&description, client->connection, false), "Could not create offer");
    EORE(rawrtc_peer_connection_set_local_description(client->connection, description), "Could not create offer");
    mem_deref(description);
  }
}

static void connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg) {
    struct client* const client = arg;

    // Print state
    default_peer_connection_state_change_handler(state, arg);

    // Only create a new channel if none exist yet (in case we disconnect and then reconnect)
    if (!client->data_channel && state == RAWRTC_PEER_CONNECTION_STATE_CONNECTED) {
        struct rawrtc_data_channel_parameters* channel_parameters;

        // Create data channel helper for in-band negotiated data channel
        data_channel_helper_create(
                &client->data_channel, (struct client *) client, "wrtc_robot_2");

        // Create data channel parameters
        EORE(rawrtc_data_channel_parameters_create(&channel_parameters, client->data_channel->label, RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, false, 0), "Could not create data channel parameters");

        // Create data channel
        EORE(rawrtc_peer_connection_create_data_channel(&client->data_channel->channel, client->connection, channel_parameters, NULL, default_data_channel_open_handler, default_data_channel_buffered_amount_low_handler, default_data_channel_error_handler, default_data_channel_close_handler, default_data_channel_message_handler, client->data_channel), "Could not create data channel");

        // Un-reference data channel parameters
        mem_deref(channel_parameters);
    }
}


// FD-listener that stops the main loop in case the input buffer contains a line feed or a carriage return.
void stop_on_return_handler(
        int flags,
        void* arg
) {
    char buffer[128];
    size_t length;
    (void) flags;
    (void) arg;

    // Get message from stdin
    if (!fgets((char*) buffer, 128, stdin)) {
      handle_errno("Could not get message from stdin", true);
    }
    length = strlen(buffer);

    // Exit?
    if (length > 0 && length < 3 && (buffer[0] == '\n' || buffer[0] == '\r')) {
        // Stop main loop
        ZF_LOGI("Exiting\n");
        re_cancel();
    }
}

void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client) {
    if (candidate) {
        const char *err_msg = "Could not print ice candidate!";
        char print_buf[1024];
        unsigned int print_len;
        enum rawrtc_code error;
        char* foundation;
        enum rawrtc_ice_protocol protocol;
        uint32_t priority;
        char* ip;
        uint16_t port;
        enum rawrtc_ice_candidate_type type;
        enum rawrtc_ice_tcp_candidate_type tcp_type;
        char const* tcp_type_str = "n/a";
        char* related_address = NULL;
        uint16_t related_port = 0;
        char* mid = NULL;
        uint8_t media_line_index = UINT8_MAX;
        char* media_line_index_str = NULL;
        char* username_fragment = NULL;
        bool is_enabled;

        // Get candidate information

        EORE(rawrtc_ice_candidate_get_foundation(&foundation, candidate), err_msg);
        EORE(rawrtc_ice_candidate_get_protocol(&protocol, candidate), err_msg);
        EORE(rawrtc_ice_candidate_get_priority(&priority, candidate), err_msg);
        EORE(rawrtc_ice_candidate_get_ip(&ip, candidate), err_msg);
        EORE(rawrtc_ice_candidate_get_port(&port, candidate), err_msg);
        EORE(rawrtc_ice_candidate_get_type(&type, candidate), err_msg);
        error = rawrtc_ice_candidate_get_tcp_type(&tcp_type, candidate);
        switch (error) {
            case RAWRTC_CODE_SUCCESS:
                tcp_type_str = rawrtc_ice_tcp_candidate_type_to_str(tcp_type);
                break;
            case RAWRTC_CODE_NO_VALUE:
                break;
            default:
                EORE(error, err_msg);
                break;
        }
        error = rawrtc_ice_candidate_get_related_address(&related_address, candidate);
        if( !(error == RAWRTC_CODE_SUCCESS || error == RAWRTC_CODE_NO_VALUE))
          handle_err(err_msg, true);
        error = rawrtc_ice_candidate_get_related_port(&related_port, candidate);
        if( !(error == RAWRTC_CODE_SUCCESS || error == RAWRTC_CODE_NO_VALUE))
          handle_err(err_msg, true);
        if (pc_candidate) {
          error = rawrtc_peer_connection_ice_candidate_get_sdp_mid(&mid, pc_candidate);
          if( !(error == RAWRTC_CODE_SUCCESS || error == RAWRTC_CODE_NO_VALUE))
            handle_err(err_msg, true);
          error = rawrtc_peer_connection_ice_candidate_get_sdp_media_line_index(
                  &media_line_index, pc_candidate);
          switch (error) {
              case RAWRTC_CODE_SUCCESS:
                  EORE(rawrtc_sdprintf(&media_line_index_str, "%"PRIu8, media_line_index), err_msg);
                  break;
              case RAWRTC_CODE_NO_VALUE:
                  break;
              default:
                  EORE(error, err_msg);
                  break;
          }
          error = rawrtc_peer_connection_ice_candidate_get_username_fragment(&username_fragment, pc_candidate);
          if( !(error == RAWRTC_CODE_SUCCESS || error == RAWRTC_CODE_NO_VALUE))
            handle_err(err_msg, true);
        }
        is_enabled = ice_candidate_type_enabled(client, type);

        // Print candidate (meh, lot's of repeated code... feel free to suggest an alternative)
        if (!pc_candidate) {
          print_len = snprintf(print_buf, 1024,
                    "(%s) ICE candidate: foundation=%s, protocol=%s"
                    ", priority=%"PRIu32", ip=%s, port=%"PRIu16", type=%s, tcp-type=%s"
                    ", related-address=%s, related-port=%"PRIu16"; URL: %s; %s\n",
                    client->name, foundation, rawrtc_ice_protocol_to_str(protocol), priority,
                    ip, port, rawrtc_ice_candidate_type_to_str(type), tcp_type_str,
                    related_address ? related_address : "n/a", related_port, url ? url : "n/a",
                    is_enabled ? "enabled" : "disabled");
        } else {
          print_len = snprintf(print_buf, 1024,
                    "(%s) ICE candidate: foundation=%s, protocol=%s"
                    ", priority=%"PRIu32", ip=%s, port=%"PRIu16", type=%s, tcp-type=%s"
                    ", related-address=%s, related-port=%"PRIu16"; URL: %s"
                    "; mid=%s, media_line_index=%s, username_fragment=%s; %s\n",
                    client->name, foundation, rawrtc_ice_protocol_to_str(protocol), priority,
                    ip, port, rawrtc_ice_candidate_type_to_str(type), tcp_type_str,
                    related_address ? related_address : "n/a", related_port, url ? url : "n/a",
                    mid ? mid : "n/a", media_line_index_str ? media_line_index_str : "n/a",
                    username_fragment ? username_fragment : "n/a",
                    is_enabled ? "enabled" : "disabled");
        }
        if( print_len > 1024){
          handle_err("ICE candidate description is too long to print", true);
        }else{
          EOE(sigserv_send(print_buf), "Could not send ICE candidate to signaling server");
        }

        // Unreference
        mem_deref(username_fragment);
        mem_deref(media_line_index_str);
        mem_deref(mid);
        mem_deref(related_address);
        mem_deref(ip);
        mem_deref(foundation);
    } else {
        ZF_LOGI("(%s) ICE gatherer last local candidate\n", client->name);
    }
}

//Create a data channel helper instance.
void data_channel_helper_create(struct data_channel_helper** const channel_helperp, struct client* const client, char* const label) {
  // Allocate
  struct data_channel_helper* const channel =
    mem_zalloc(sizeof(*channel), data_channel_helper_destroy);
  if (!channel) {
    EORE(RAWRTC_CODE_NO_MEMORY, "Could not allocate memory for data channel helper.");
    return;
  }

  // Set fields
  channel->client = client;
  EORE(rawrtc_strdup(&channel->label, label), "Coudl not copy label into new data channel helper");

  // Set pointer & done
  *channel_helperp = channel;
}

//Destroy a data channel helper instance.
static void data_channel_helper_destroy(void* arg) {
  struct data_channel_helper* const channel = arg;

  // Unset handler argument & handlers of the channel
  if (channel->channel) {
    EORE(rawrtc_data_channel_unset_handlers(channel->channel), "Could not unset handlers while destroying data channel helper");
  }

  // Remove from list
  list_unlink(&channel->le);

  // Un-reference
  mem_deref(channel->arg);
  mem_deref(channel->label);
  mem_deref(channel->channel);
}

//Check if the ICE candidate type is enabled.
bool ice_candidate_type_enabled(struct client* const client, enum rawrtc_ice_candidate_type const type) {
  char const* const type_str = rawrtc_ice_candidate_type_to_str(type);
  size_t i;

  // All enabled?
  if (client->n_ice_candidate_types == 0) {
    return true;
  }

  // Specifically enabled?
  for (i = 0; i < client->n_ice_candidate_types; ++i) {
    if (str_cmp(client->ice_candidate_types[i], type_str) == 0) {
      return true;
    }
  }

  // Nope
  return false;
}

static void parse_remote_description(int flags, void* arg) {
/*     struct peer_connection_client* const client = arg; */
/*     enum rawrtc_code error; */
/*     bool do_exit = false; */
/*     struct odict* dict = NULL; */
/*     char* type_str; */
/*     char* sdp; */
/*     enum rawrtc_sdp_type type; */
/*     struct rawrtc_peer_connection_description* remote_description = NULL; */
/*     (void) flags; */

/*     // Get dict from JSON */
/*     error = get_json_stdin(&dict); */
/*     if (error) { */
/*         do_exit = error == RAWRTC_CODE_NO_VALUE; */
/*         goto out; */
/*     } */

/*     // Decode JSON */
/*     error |= dict_get_entry(&type_str, dict, "type", ODICT_STRING, true); */
/*     error |= dict_get_entry(&sdp, dict, "sdp", ODICT_STRING, true); */
/*     if (error) { */
/*         DEBUG_WARNING("Invalid remote description\n"); */
/*         goto out; */
/*     } */

/*     // Convert to description */
/*     error = rawrtc_str_to_sdp_type(&type, type_str); */
/*     if (error) { */
/*         DEBUG_WARNING("Invalid SDP type in remote description: '%s'\n", type_str); */
/*         goto out; */
/*     } */
/*     error = rawrtc_peer_connection_description_create(&remote_description, type, sdp); */
/*     if (error) { */
/*         DEBUG_WARNING("Cannot parse remote description: %s\n", rawrtc_code_to_str(error)); */
/*         goto out; */
/*     } */

/*     // Set remote description */
/*     DEBUG_INFO("Applying remote description\n"); */
/*     EOE(rawrtc_peer_connection_set_remote_description(client->connection, remote_description)); */

/*     // Answering: Create and set local description */
/*     if (!client->offering) { */
/*         struct rawrtc_peer_connection_description* local_description; */
/*         EOE(rawrtc_peer_connection_create_answer(&local_description, client->connection)); */
/*         EOE(rawrtc_peer_connection_set_local_description(client->connection, local_description)); */
/*         mem_deref(local_description); */
/*     } */

/* out: */
/*     // Un-reference */
/*     mem_deref(remote_description); */
/*     mem_deref(dict); */

/*     // Exit? */
/*     if (do_exit) { */
/*         DEBUG_NOTICE("Exiting\n"); */

/*         // Stop client & bye */
/*         client_stop(client); */
/*         tmr_cancel(&timer); */
/*         before_exit(); */
/*         exit(0); */
/*     } */
}
