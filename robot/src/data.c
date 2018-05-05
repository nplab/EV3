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
static void local_candidate_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, void* const arg);
static void send_local_description(struct client* const client);
void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client);
void data_channel_helper_create(struct data_channel_helper** const channel_helperp, struct client* const client, char* const label);
static void data_channel_helper_destroy(void* arg);
bool ice_candidate_type_enabled(struct client* const client, enum rawrtc_ice_candidate_type const type);
static void get_remote_description();
void api_channel_open_handler(void* const arg);

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


  if( *stun_urls != NULL){
    EORE(rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, stun_urls, stun_urls_length,
                                                          NULL, NULL, RAWRTC_ICE_CREDENTIAL_TYPE_NONE), "Could not add STUN servers to RawRTC configuration");
  }

  if( *turn_urls != NULL){
    EORE(rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, turn_urls, turn_urls_length,
                                                          "threema-angular", "Uv0LcCq3kyx6EiRwQW5jVigkhzbp70CjN2CJqzmRxG3UGIdJHSJV6tpo7Gj7YnGB",
                                                          RAWRTC_ICE_CREDENTIAL_TYPE_PASSWORD), "Could not add TURN servers to  RawRTC configuration");
  }

  //set up client information
  client_info.name = "WebRTCRobot";
  client_info.ice_candidate_types = NULL;
  client_info.n_ice_candidate_types = 0;
  client_info.configuration = configuration;
  client_info.offering = true;

  initialise_client();
  get_remote_description();

  re_main(default_signal_handler);

  return WRTCR_SUCCESS;
}

wrtcr_rc data_channel_shutdown(){
  client_stop();
  rawrtc_close();
  tmr_debug();
  mem_debug();
  return WRTCR_SUCCESS;
}

wrtcr_rc initialise_client(){
  struct rawrtc_data_channel_parameters* dc_parameters;
  //create peer connection
  EORE(rawrtc_peer_connection_create(
                                    &client_info.connection, configuration,
                                    negotiation_needed_handler, local_candidate_handler,
                                    default_peer_connection_local_candidate_error_handler,
                                    default_signaling_state_change_handler, default_ice_transport_state_change_handler,
                                    default_ice_gatherer_state_change_handler, connection_state_change_handler,
                                    default_data_channel_handler, &client_info), "Could not create peer connection");

  //create parameters for data channel
  data_channel_helper_create(&client_info.data_channel_negotiated,  &client_info, "api");

  EORE(rawrtc_data_channel_parameters_create(
                                           &dc_parameters, client_info.data_channel_negotiated->label,
                                           RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, true, 0), "Could not create data channel parameters");

  //create actual data channel
  EORE(rawrtc_peer_connection_create_data_channel(
                                                &client_info.data_channel_negotiated->channel, client_info.connection,
                                                dc_parameters, NULL,
                                                api_channel_open_handler, default_data_channel_buffered_amount_low_handler,
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
  ZF_LOGI("(%s) Negotiation needed\n", client->name);

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

    //print the new state
    char const * const state_name = rawrtc_peer_connection_state_to_name(state);
    ZF_LOGD("(%s) Peer connection state change: %s\n", client->name, state_name);

    // Only create a new channel if none exist yet (in case we disconnect and then reconnect)
    /* if (!client->data_channel && state == RAWRTC_PEER_CONNECTION_STATE_CONNECTED) { */
    /*     struct rawrtc_data_channel_parameters* channel_parameters; */

    /*     // Create data channel helper for in-band negotiated data channel */
    /*     data_channel_helper_create( */
    /*             &client->data_channel, (struct client *) client, "wrtc_robot_2"); */

    /*     // Create data channel parameters */
    /*     EORE(rawrtc_data_channel_parameters_create(&channel_parameters, client->data_channel->label, RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, false, 0), "Could not create data channel parameters"); */

    /*     // Create data channel */
    /*     EORE(rawrtc_peer_connection_create_data_channel(&client->data_channel->channel, client->connection, channel_parameters, NULL, default_data_channel_open_handler, default_data_channel_buffered_amount_low_handler, default_data_channel_error_handler, default_data_channel_close_handler, default_data_channel_message_handler, client->data_channel), "Could not create data channel"); */

    /*     // Un-reference data channel parameters */
    /*     mem_deref(channel_parameters); */
    /* } */
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

//handle local candidata (print, send last one to signaling server)
static void local_candidate_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, void* const arg) {
  struct client* const client = arg;

  // Print local candidate
  default_peer_connection_local_candidate_handler(candidate, url, arg);

  // Print local description (if last candidate)
  if(!candidate) {
    send_local_description(client);
  }
}

static void send_local_description(struct client* const client) {
  struct rawrtc_peer_connection_description* description;
  enum rawrtc_sdp_type type;
  char* sdp;
  cJSON* root;
  char * output;
  char * err_msg = "Could not create JSON for local description";

  // Get description
  EORE(rawrtc_peer_connection_get_local_description(&description, client->connection), "Could not get local description");

  // Get SDP type & the SDP itself
  EORE(rawrtc_peer_connection_description_get_sdp_type(&type, description), "Could not get sdp type");
  EORE(rawrtc_peer_connection_description_get_sdp(&sdp, description), "Could not get SDP");

  // Create json
  root = cJSON_CreateObject();
  EON(root, err_msg);
  cJSON *temp = cJSON_CreateString(rawrtc_sdp_type_to_str(type));
  EON(temp, err_msg);
  cJSON_AddItemToObject(root, "type", temp);
  temp = cJSON_CreateString(sdp);
  EON(temp, err_msg);
  cJSON_AddItemToObject(root, "sdp", temp);

  // Print local description as JSON
  output = cJSON_Print(root);
  ZF_LOGD("Local Description:%s", output);
  sigserv_send(output);

  // Un-reference
  cJSON_Delete(root);
  free(output);
  mem_deref(sdp);
  mem_deref(description);
}

void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client) {
    if (candidate) {
        const char *err_msg = "Could not print ice candidate!";
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
            ZF_LOGI(
                    "(%s) ICE candidate: foundation=%s, protocol=%s"
                    ", priority=%"PRIu32", ip=%s, port=%"PRIu16", type=%s, tcp-type=%s"
                    ", related-address=%s, related-port=%"PRIu16"; URL: %s; %s\n",
                    client->name, foundation, rawrtc_ice_protocol_to_str(protocol), priority,
                    ip, port, rawrtc_ice_candidate_type_to_str(type), tcp_type_str,
                    related_address ? related_address : "n/a", related_port, url ? url : "n/a",
                    is_enabled ? "enabled" : "disabled");
        } else {
            ZF_LOGI(
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
  EORE(rawrtc_strdup(&channel->label, label), "Could not copy label into new data channel helper");

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

static void get_remote_description() {
    enum rawrtc_code error;
    bool do_exit = false;
    char *type_str = NULL;
    char *sdp_str = NULL;
    char *input = NULL;
    cJSON* root = NULL;
    enum rawrtc_sdp_type type;
    struct rawrtc_peer_connection_description* remote_description = NULL;


    EOE(sigserv_receive(&input), "Could not get remote description from signaling server");
    root = cJSON_Parse(input);
    if (!root) {
      ZF_LOGE("Could not parse remote description JSON");
      do_exit = true;
      goto out;
    }


    cJSON *temp = cJSON_GetObjectItem(root, "type");
    if( !cJSON_IsString(temp) && temp->valuestring == NULL){
      ZF_LOGE("Invalid remote description\n");
      goto out;
    }
    type_str = temp->valuestring;

    temp = cJSON_GetObjectItem(root, "sdp");
    if( !cJSON_IsString(temp) && temp->valuestring == NULL){
      ZF_LOGE("Invalid remote description\n");
      goto out;
    }
    sdp_str = temp->valuestring;

    // Convert to description
    error = rawrtc_str_to_sdp_type(&type, type_str);
    if (error) {
        ZF_LOGE("Invalid SDP type in remote description: '%s'\n", type_str);
        goto out;
    }
    error = rawrtc_peer_connection_description_create(&remote_description, type, sdp_str);
    if (error) {
        ZF_LOGE("Cannot parse remote description: %s\n", rawrtc_code_to_str(error));
        goto out;
    }

    // Set remote description
    ZF_LOGI("Applying remote description\n");
    EORE(rawrtc_peer_connection_set_remote_description(client_info.connection, remote_description), "Could not apply remote description");

out:
    // Un-reference
    mem_deref(remote_description);
    free(input);
    cJSON_Delete(root);

    // Exit?
    if (do_exit) {
        ZF_LOGI("Exiting\n");

        // Stop client_info & bye
        /* tmr_cancel(&timer); */
        data_channel_shutdown();
        exit(0);
    }
}

void api_channel_open_handler(void* const arg) {
  struct data_channel_helper* const channel = arg;
  char *description = NULL;
  ZF_LOGI("(%s) Data channel open: %s\n", channel->client->name, channel->label);

  EOE(get_port_description(&description), "Could not get port description");

  struct mbuf *desc_mbuf = mbuf_alloc(strlen(description)+1);
  mbuf_printf(desc_mbuf, "%s", description);
  mbuf_set_pos(desc_mbuf, 0);

  free(description);

  EORE(rawrtc_data_channel_send(channel->channel,  desc_mbuf, false), "Could not send port description message");
}
