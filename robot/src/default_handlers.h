#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <rawrtc.h>
#pragma GCC diagnostic pop

#include "data.h"

void default_ice_gatherer_state_change_handler(enum rawrtc_ice_gatherer_state const state, void* const arg);

void default_ice_gatherer_error_handler(struct rawrtc_ice_candidate* const host_candidate, char const * const url, uint16_t const error_code, char const * const error_text, void* const arg);

void default_ice_gatherer_local_candidate_handler(struct rawrtc_ice_candidate* const candidate, char const * const url, void* const arg);

void default_ice_transport_state_change_handler(enum rawrtc_ice_transport_state const state, void* const arg);

void default_ice_transport_candidate_pair_change_handler(struct rawrtc_ice_candidate* const local, struct rawrtc_ice_candidate* const remote, void* const arg);

void default_dtls_transport_state_change_handler(enum rawrtc_dtls_transport_state const state, void* const arg);

void default_dtls_transport_error_handler(void* const arg);

void default_sctp_transport_state_change_handler(enum rawrtc_sctp_transport_state const state, void* const arg);

void default_data_channel_handler(struct rawrtc_data_channel* const data_channel, void* const arg);

void default_data_channel_open_handler(void* const arg);

void default_data_channel_buffered_amount_low_handler(void* const arg);

void default_data_channel_error_handler(void* const arg);

void default_data_channel_close_handler(void* const arg);

void default_data_channel_message_handler(struct mbuf* const buffer, enum rawrtc_data_channel_message_flag const flags, void* const arg);

void default_negotiation_needed_handler(void* const arg);

void default_peer_connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg);

void default_peer_connection_local_candidate_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, void* const arg);

void default_peer_connection_local_candidate_error_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, uint16_t const error_code, char const * const error_text, void* const arg);

void default_signaling_state_change_handler(enum rawrtc_signaling_state const state, void* const arg);

void default_signal_handler(int sig); 
