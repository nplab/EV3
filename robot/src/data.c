#include <pthread.h>
#include <time.h>
#include "data.h"

//connection configuration
static struct client client_info = {0};
static struct rawrtc_peer_connection_configuration *configuration;
//vars for ping checking
static pthread_t ping_thread_id;
static time_t ping_ts;
//index of sensor messages
static unsigned long sensor_idx = 0;

//initialisation and shutdown functions
wrtcr_rc initialise_client();
wrtcr_rc client_stop();

//signal handlers
static void negotiation_needed_handler(void* const arg);
static void connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg);
static void local_candidate_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, void* const arg);
wrtcr_rc send_local_description(struct rawrtc_peer_connection_description* description);
void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client);
void data_channel_helper_create(struct data_channel_helper** const channel_helperp, struct client* const client, char* const label);
static void data_channel_helper_destroy(void* arg);
bool ice_candidate_type_enabled(struct client* const client, enum rawrtc_ice_candidate_type const type);
static void get_remote_info();
static void handle_remote_description(cJSON *desc_json);
int handle_remote_ICE_candidate(cJSON *candidate_json);
void api_channel_open_handler(void* const arg);
void ping_channel_open_handler(void* const arg);
void robot_api_message_handler(struct mbuf* const buffer, enum rawrtc_data_channel_message_flag const flags, void* const arg);
//helper functions
wrtcr_rc send_message_on_data_channel(struct rawrtc_data_channel* channel, char *msg);
void* ping_routine(void *p_channel);
void ping_message_handler(struct mbuf* const buffer, enum rawrtc_data_channel_message_flag const flags __attribute__ ((unused)), void* const arg __attribute__ ((unused)));


wrtcr_rc data_channel_setup(){
  unsigned int stun_urls_length = 0;
  char **stun_urls;
  unsigned int turn_urls_length = 0;
  char **turn_urls;


  //initialize RawRTC
  EORE(rawrtc_init(),"RawRTC Initialisaton failed");

  //set up configuration
  EORE(rawrtc_peer_connection_configuration_create(&configuration, RAWRTC_ICE_GATHER_POLICY_ALL), "Could not create RawRTC configuration");

  //get servers from config
  if(conf_get_string_array("stun_servs", &stun_urls, &stun_urls_length) == WRTCR_SUCCESS && stun_urls_length != 0){
    EORE(rawrtc_peer_connection_configuration_add_ice_server(
                                                             configuration, stun_urls, stun_urls_length,
                                                             NULL, NULL, RAWRTC_ICE_CREDENTIAL_TYPE_NONE), "Could not add STUN servers to RawRTC configuration");
  }

  if(conf_get_string_array("turn_servs", &turn_urls, &turn_urls_length) == WRTCR_SUCCESS && turn_urls_length != 0){
    EORE(rawrtc_peer_connection_configuration_add_ice_server(
                                                             configuration, turn_urls, turn_urls_length,
                                                             NULL, NULL, RAWRTC_ICE_CREDENTIAL_TYPE_NONE), "Could not add TURN servers to  RawRTC configuration");
  }

  //set up client information
  client_info.name = "WebRTCRobot";
  client_info.ice_candidate_types = NULL;
  client_info.n_ice_candidate_types = 0;
  client_info.configuration = configuration;
  client_info.offering = true;

  initialise_client();
  get_remote_info();

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

  //----------------------------- create API data channel ---------------------------------------
  //create parameters for api data channel
  data_channel_helper_create(&client_info.data_channel_api,  &client_info, "api");

  EORE(rawrtc_data_channel_parameters_create(
                                           &dc_parameters, client_info.data_channel_api->label,
                                           RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED, 0, NULL, true, 0), "Could not create api data channel parameters");

  //create actual data channel
  EORE(rawrtc_peer_connection_create_data_channel(
                                                &client_info.data_channel_api->channel, client_info.connection,
                                                dc_parameters, NULL,
                                                api_channel_open_handler, default_data_channel_buffered_amount_low_handler,
                                                default_data_channel_error_handler, default_data_channel_close_handler,
                                                robot_api_message_handler, client_info.data_channel_api), "Could not create api data channel");
  //clean up
  mem_deref(dc_parameters);

  //----------------------------- create sensors data channel ---------------------------------------
  //create parameters for sensor data channel
  data_channel_helper_create(&client_info.data_channel_sensors, &client_info, "sensors");

  EORE(rawrtc_data_channel_parameters_create(
                                           &dc_parameters, client_info.data_channel_sensors->label,
                                           RAWRTC_DATA_CHANNEL_TYPE_UNRELIABLE_ORDERED_RETRANSMIT, 0, NULL, true, 1), "Could not create sensors data channel parameters");

  //create actual data channel
  EORE(rawrtc_peer_connection_create_data_channel(
                                                &client_info.data_channel_sensors->channel, client_info.connection,
                                                dc_parameters, NULL,
                                                default_data_channel_open_handler, default_data_channel_buffered_amount_low_handler,
                                                default_data_channel_error_handler, default_data_channel_close_handler,
                                                default_data_channel_message_handler, client_info.data_channel_sensors), "Could not create sensors data channel");

  //clean up
  mem_deref(dc_parameters);

  //----------------------------- create ping data channel ---------------------------------------
  //create parameters for ping data channel
  data_channel_helper_create(&client_info.data_channel_ping,  &client_info, "ping");

  EORE(rawrtc_data_channel_parameters_create(
                                           &dc_parameters, client_info.data_channel_ping->label,
                                           RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_UNORDERED, 0, NULL, true, 2), "Could not create ping data channel parameters");

  //create actual data channel
  EORE(rawrtc_peer_connection_create_data_channel(
                                                &client_info.data_channel_ping->channel, client_info.connection,
                                                dc_parameters, NULL,
                                                ping_channel_open_handler, default_data_channel_buffered_amount_low_handler,
                                                default_data_channel_error_handler, default_data_channel_close_handler,
                                                ping_message_handler, client_info.data_channel_ping), "Could not create ping data channel");
  //more clean up
  mem_deref(dc_parameters);

  return WRTCR_SUCCESS;
}

wrtcr_rc client_stop(){
  wrtcr_rc rc = WRTCR_SUCCESS;
  if(rawrtc_peer_connection_close(client_info.connection) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not close data channel", false);
    rc = WRTCR_FAILURE;
  }
  client_info.data_channel_api = mem_deref(client_info.data_channel_api);
  client_info.data_channel_sensors = mem_deref(client_info.data_channel_sensors);
  client_info.data_channel_ping = mem_deref(client_info.data_channel_ping);
  client_info.connection = mem_deref(client_info.connection);
  client_info.configuration = mem_deref(client_info.configuration);

  return rc;
}

static void negotiation_needed_handler(void* const arg) {
  struct client* const client = arg;

  ZF_LOGI("(%s) Negotiation needed\n", client->name);

  // Offering: Create and set local description
  if (client->offering) {
    struct rawrtc_peer_connection_description* description;
    //make sure to send the local description before setting, since setting it causes the first ICE candidate to be sent
    //this is a bug, see https://github.com/rawrtc/rawrtc/issues/116
    EORE(rawrtc_peer_connection_create_offer(&description, client->connection, false), "Could not create offer");
    EOE(send_local_description(description), "Could not send local description");
    EORE(rawrtc_peer_connection_set_local_description(client->connection, description), "Could not create offer");
    mem_deref(description);
  }
}

static void connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg) {
    struct client* const client = arg;

    //print the new state
    char const * const state_name = rawrtc_peer_connection_state_to_name(state);
    ZF_LOGD("(%s) Peer connection state change: %s\n", client->name, state_name);
}

//handle local candidate 
static void local_candidate_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, void* const arg) {
  cJSON *root;

  if (candidate) {
    char *candidate_sdp = NULL, *mid = NULL, *msg=NULL;
    uint8_t mli;
    root = cJSON_CreateObject();

    EORE(rawrtc_peer_connection_ice_candidate_get_sdp(&candidate_sdp, candidate), "Could not get sdp string for local ICE candidate");
    cJSON_AddStringToObject(root, "candidate", candidate_sdp);
    mem_deref(candidate_sdp);

    if(rawrtc_peer_connection_ice_candidate_get_sdp_mid(&mid, candidate) != RAWRTC_CODE_NO_VALUE){
      cJSON_AddStringToObject(root, "sdpMid", mid);
      mem_deref(mid);
    }
    if(rawrtc_peer_connection_ice_candidate_get_sdp_media_line_index(&mli, candidate) != RAWRTC_CODE_NO_VALUE){
      cJSON_AddItemToObject(root, "sdpMLineIndex", cJSON_CreateNumber(mli));
    }

    msg = cJSON_Print(root);
    sigserv_send(msg);

    free(msg);
    cJSON_Delete(root);
  }
  // Print local candidate
  default_peer_connection_local_candidate_handler(candidate, url, arg);

}

wrtcr_rc send_local_description(struct rawrtc_peer_connection_description* description) {
  wrtcr_rc ret = WRTCR_SUCCESS;
  enum rawrtc_sdp_type type;
  char* sdp;
  cJSON* root;
  char * output;
  char * err_msg = "Could not create JSON for local description";

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
  ret = sigserv_send(output);

  // Un-reference
  cJSON_Delete(root);
  free(output);
  mem_deref(sdp);

  return ret;
}

void print_ice_candidate(struct rawrtc_ice_candidate* const candidate, char const* const url, struct rawrtc_peer_connection_ice_candidate* const pc_candidate, struct client* const client) {
    if(candidate) {
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

static void get_remote_info(){
  cJSON* root = NULL;
  char *input = NULL;

  EOE(sigserv_receive(&input), "Could not get remote information from signaling server");
  while((root = cJSON_Parse(input))){
    if(cJSON_GetObjectItem(root, "candidate") != NULL){
      if( handle_remote_ICE_candidate(root) ){
        break;
      }
    } else if(cJSON_GetObjectItem(root, "type") != NULL){
      handle_remote_description(root);
    } else{
      ZF_LOGE("Unknown type of remote information received.");
    }
    free(input);
    cJSON_Delete(root);
    EOE(sigserv_receive(&input), "Could not get remote information from signaling server");
  }
}

int handle_remote_ICE_candidate(cJSON *candidate_json){
  struct rawrtc_peer_connection_ice_candidate* candidate;
  cJSON *sdp_item, *mid_item, *mli_item;
  if(!(sdp_item = cJSON_GetObjectItem(candidate_json, "candidate"))){
    ZF_LOGE("Could not get sdp string from remote ICE candidate JSON");
  }
  if(strlen(sdp_item->valuestring) == 0){ //last (empty) candidate
    return 1;
  }
  if(!(mid_item = cJSON_GetObjectItem(candidate_json, "sdpMid"))){
    ZF_LOGE("Could not get sdpMid string from remote ICE candidate JSON");
  }
  if(!(mli_item = cJSON_GetObjectItem(candidate_json, "sdpMLineIndex"))){
    ZF_LOGE("Could not get sdpMLineIndex integer from remote ICE candidate JSON");
  }

  EORE(rawrtc_peer_connection_ice_candidate_create(&candidate, sdp_item->valuestring, mid_item->valuestring, (const uint8_t * const)&(mli_item->valueint), NULL), "Could not create remote ICE candidate");

  ZF_LOGD("Adding remote ICE candidate");
  EORE(rawrtc_peer_connection_add_ice_candidate(client_info.connection, candidate), "Could not set remote ICE candidate");
  mem_deref(candidate);
  return 0;
}

static void handle_remote_description(cJSON *desc_json) {
    enum rawrtc_code error;
    char *type_str = NULL;
    char *sdp_str = NULL;
    enum rawrtc_sdp_type type;
    struct rawrtc_peer_connection_description* remote_description = NULL;

    cJSON *temp = cJSON_GetObjectItem(desc_json, "type");
    if( !cJSON_IsString(temp) && temp->valuestring == NULL){
      ZF_LOGE("Invalid remote description\n");
      goto out;
    }
    type_str = temp->valuestring;

    temp = cJSON_GetObjectItem(desc_json, "sdp");
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
}

void api_channel_open_handler(void* const arg) {
  struct data_channel_helper* const channel = arg;
  char *description = NULL;
  ZF_LOGI("(%s) Data channel open: %s\n", channel->client->name, channel->label);

  EOE(get_port_description(&description), "Could not get port description");

  EOE(send_message_on_api_channel(description), "Could not send port description message");

  free(description);
}

void robot_api_message_handler(struct mbuf* const buffer, enum rawrtc_data_channel_message_flag const flags, void* const arg) {
  struct data_channel_helper* const channel = arg;
  struct client* const client = channel->client;
  (void) flags;

  ZF_LOGD("(%s) Incoming message for data channel %s: %zu bytes\n",
          client->name, channel->label, mbuf_get_left(buffer));

  //parse JSON and check it
  cJSON *root = cJSON_Parse((const char*)buffer->buf);
  if(!root){
    ZF_LOGI("Could not parse JSON on channel %s", channel->label);
    return;
  }
  cJSON *port_item = cJSON_GetObjectItem(root, "port");
  char *port = cJSON_GetStringValue(port_item);
  if(port == NULL || strlen(port) != 1){
    handle_err("Message malformed, no port", false);
    return;
  }

  //call handler functions based on port
  if( *port >= 'a'){
    handle_meta_device_message(port, root);
  } else if ( *port >= 'A'){
    handle_tacho_message(port, root);
  } else {
    handle_sensor_message(port, root);
  }
  cJSON_Delete(root);
  return;
}

wrtcr_rc send_message_on_data_channel(struct rawrtc_data_channel* channel, char *msg){
  struct mbuf *buf = mbuf_alloc(strlen(msg)+1);
  mbuf_printf(buf, "%s", msg);
  mbuf_set_pos(buf, 0);

  //if the channel is open, try to send data on it
  if(channel->state == RAWRTC_DATA_CHANNEL_STATE_OPEN && rawrtc_data_channel_send(channel,  buf, false) == RAWRTC_CODE_SUCCESS){
    mem_deref(buf);
    return WRTCR_SUCCESS;
  } else {
    mem_deref(buf);
    return WRTCR_FAILURE;
  }
}

wrtcr_rc send_message_on_ping_channel(char *msg){
  if(send_message_on_data_channel(client_info.data_channel_ping->channel, msg) != WRTCR_SUCCESS){
    ZF_LOGE("Could not send message on ping channel");
    return WRTCR_FAILURE;
  }
  return WRTCR_SUCCESS;
}

wrtcr_rc send_message_on_api_channel(char *msg){
  if(send_message_on_data_channel(client_info.data_channel_api->channel, msg) != WRTCR_SUCCESS){
    ZF_LOGE("Could not send message on api channel");
    return WRTCR_FAILURE;
  }
  return WRTCR_SUCCESS;
}

wrtcr_rc send_message_on_sensor_channel(char *port, int value1){
  static char msg[100];
  sensor_idx++;

  snprintf(msg, sizeof(msg), "{\"idx\": %lu, \"port\": \"%s\", \"value\": [%d]}", sensor_idx, port, value1);
  if(send_message_on_data_channel(client_info.data_channel_sensors->channel, msg) != WRTCR_SUCCESS){
    ZF_LOGE("Could not send message on sensor channel");
    return WRTCR_FAILURE;
  }
  return WRTCR_SUCCESS;
}

wrtcr_rc send_long_message_on_sensor_channel(char *port, int value1, int value2){
  static char msg[120];
  sensor_idx++;

  snprintf(msg, sizeof(msg), "{\"idx\": %lu, \"port\": \"%s\", \"value\": [%d, %d]}", sensor_idx, port, value1, value2);
  if(send_message_on_data_channel(client_info.data_channel_sensors->channel, msg) != WRTCR_SUCCESS){
    ZF_LOGE("Could not send message on sensor channel");
    return WRTCR_FAILURE;
  }
  return WRTCR_SUCCESS;
}

void ping_channel_open_handler(void* const arg){
  struct data_channel_helper* const channel = arg;
  ZF_LOGI("(%s) Data channel open: %s\n", channel->client->name, channel->label);

  time(&ping_ts);//initialise timestamp
  if(pthread_create(&ping_thread_id, NULL, &ping_routine, &ping_ts) != 0){
    ZF_LOGE("Could not create ping thread. Exiting!");
    exit(-1);
  }
}

void ping_message_handler(struct mbuf* const buffer __attribute__ ((unused)), enum rawrtc_data_channel_message_flag const flags __attribute__ ((unused)), void* const arg __attribute__ ((unused))) {
  time(&ping_ts);
}

