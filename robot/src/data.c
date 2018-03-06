#include "data.h"

static struct peer_connection_client client = {0};

wrtcr_rc data_channel_setup(){
  unsigned int stun_urls_length;
  char **stun_urls;
  unsigned int turn_urls_length;
  char **turn_urls;
  struct rawrtc_peer_connection_configuration *configuration;

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
    handle_err("Could not create RawRTC configuration", true);
  }

  if( rawrtc_peer_connection_configuration_add_ice_server(
                                                          configuration, turn_urls, turn_urls_length,
                                                          "threema-angular", "Uv0LcCq3kyx6EiRwQW5jVigkhzbp70CjN2CJqzmRxG3UGIdJHSJV6tpo7Gj7YnGB",
                                                          RAWRTC_ICE_CREDENTIAL_TYPE_PASSWORD) != RAWRTC_CODE_SUCCESS){
    handle_err("Could not create RawRTC configuration", true);
  }

  return WRTCR_SUCCESS;
}

wrtcr_rc data_channel_shutdown(){
  /* client_stop(&client); */
  rawrtc_close();
  /* tmr_debug(); */
  /* mem_debug(); */
  return WRTCR_SUCCESS;
}
