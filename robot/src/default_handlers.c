#include "default_handlers.h"

// Print the ICE gatherer's state.
void default_ice_gatherer_state_change_handler(enum rawrtc_ice_gatherer_state const state, void* const arg) {
    struct client* const client = arg;
    char const * const state_name = rawrtc_ice_gatherer_state_to_name(state);
    (void) arg;
    ZF_LOGD("(%s) ICE gatherer state: %s\n", client->name, state_name);
}

// Print the ICE gatherer's error event.
void default_ice_gatherer_error_handler(struct rawrtc_ice_candidate* const candidate, char const * const url, uint16_t const error_code, char const * const error_text, void* const arg) {
    struct client* const client = arg;
    (void) candidate; (void) error_code; (void) arg;
    ZF_LOGI("(%s) ICE gatherer error, URL: %s, reason: %s\n", client->name, url, error_text);
}

// Print the newly gathered local candidate.
void default_ice_gatherer_local_candidate_handler(struct rawrtc_ice_candidate* const candidate, char const * const url, void* const arg) {
    struct client* const client = arg;
    (void) arg;
    print_ice_candidate(candidate, url, NULL, client);
}


// Print the ICE transport's state.
void default_ice_transport_state_change_handler(enum rawrtc_ice_transport_state const state, void* const arg) {
    struct client* const client = arg;
    char const * const state_name = rawrtc_ice_transport_state_to_name(state);
    (void) arg;
    ZF_LOGD("(%s) ICE transport state: %s\n", client->name, state_name);
}

// Print the ICE candidate pair change event.
void default_ice_transport_candidate_pair_change_handler(struct rawrtc_ice_candidate* const local, struct rawrtc_ice_candidate* const remote, void* const arg) {
    struct client* const client = arg;
    (void) local; (void) remote;
    ZF_LOGD("(%s) ICE transport candidate pair change\n", client->name);
}

// Print the DTLS transport's state.
void default_dtls_transport_state_change_handler(enum rawrtc_dtls_transport_state const state, void* const arg) {
    struct client* const client = arg;
    char const * const state_name = rawrtc_dtls_transport_state_to_name(state);
    ZF_LOGD("(%s) DTLS transport state change: %s\n", client->name, state_name);
}

// Print the DTLS transport's error event.
void default_dtls_transport_error_handler(void* const arg) {
    struct client* const client = arg;
    ZF_LOGW("(%s) DTLS transport error: %s\n", client->name, "???");
}

// Print the SCTP transport's state.
void default_sctp_transport_state_change_handler(enum rawrtc_sctp_transport_state const state, void* const arg) {
    struct client* const client = arg;
    char const * const state_name = rawrtc_sctp_transport_state_to_name(state);
    ZF_LOGD("(%s) SCTP transport state change: %s\n", client->name, state_name);
}


// Print the newly created data channel's parameter.
void default_data_channel_handler(struct rawrtc_data_channel* const channel, void* const arg) {
    struct client* const client = arg;
    struct rawrtc_data_channel_parameters* parameters;
    char* label = NULL;

    // Get data channel label and protocol
    EORE(rawrtc_data_channel_get_parameters(&parameters, channel), "Could not get data channel parameters");
    EORE(rawrtc_data_channel_parameters_get_label(&label, parameters), "Could not get data channel label");
    ZF_LOGI("(%s) New data channel instance: %s\n", client->name, label ? label : "n/a");
    mem_deref(label);
    mem_deref(parameters);
}

// Print the data channel open event.
void default_data_channel_open_handler(void* const arg) {
    struct data_channel_helper* const channel = arg;
    struct client* const client = channel->client;
    ZF_LOGD("(%s) Data channel open: %s\n", client->name, channel->label);
}


// Print the data channel buffered amount low event.
void default_data_channel_buffered_amount_low_handler(void* const arg) {
    struct data_channel_helper* const channel = arg;
    struct client* const client = channel->client;
    ZF_LOGD("(%s) Data channel buffered amount low: %s\n", client->name, channel->label);
}

// Print the data channel error event.
void default_data_channel_error_handler(void* const arg) {
    struct data_channel_helper* const channel = arg;
    struct client* const client = channel->client;
    ZF_LOGW("(%s) Data channel error: %s\n", client->name, channel->label);
}

// Print the data channel close event.
void default_data_channel_close_handler(void* const arg) {
    struct data_channel_helper* const channel = arg;
    struct client* const client = channel->client;
    ZF_LOGD("(%s) Data channel closed: %s\n", client->name, channel->label);
}

// Print the data channel's received message's size.
void default_data_channel_message_handler(struct mbuf* const buffer, enum rawrtc_data_channel_message_flag const flags, void* const arg) {
    struct data_channel_helper* const channel = arg;
    struct client* const client = channel->client;
    (void) flags;
    ZF_LOGD("(%s) Incoming message for data channel %s: %zu bytes\n",
                 client->name, channel->label, mbuf_get_left(buffer));
}

// Print negotiation needed (duh!)
void default_negotiation_needed_handler(void* const arg) {
    struct client* const client = arg;
    ZF_LOGD("(%s) Negotiation needed\n", client->name);
}

// Print the peer connection's state.
void default_peer_connection_state_change_handler(enum rawrtc_peer_connection_state const state, void* const arg) {
    struct client* const client = arg;
    char const * const state_name = rawrtc_peer_connection_state_to_name(state);
    ZF_LOGD("(%s) Peer connection state change: %s\n", client->name, state_name);
}

// Print the newly gathered local candidate (peer connection variant).
void default_peer_connection_local_candidate_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, void* const arg) {
    struct client* const client = arg;
    struct rawrtc_ice_candidate* ortc_candidate = NULL;

    // Get underlying ORTC ICE candidate (if any)
    if (candidate) {
      EORE(rawrtc_peer_connection_ice_candidate_get_ortc_candidate(&ortc_candidate, candidate), "Could not get ORTC ICE candidate");
    }

    // Print local candidate
    print_ice_candidate(ortc_candidate, url, candidate, client);
    mem_deref(ortc_candidate);
}

// Print the peer connections local candidate error event.
void default_peer_connection_local_candidate_error_handler(struct rawrtc_peer_connection_ice_candidate* const candidate, char const * const url, uint16_t const error_code, char const * const error_text, void* const arg) {
    struct client* const client = arg;
    (void) candidate; (void) error_code; (void) arg;
    ZF_LOGI("(%s) ICE candidate error, URL: %s, reason: %s\n", client->name, url, error_text);
}

// Print the signaling state.
void default_signaling_state_change_handler(enum rawrtc_signaling_state const state, void* const arg) {
    struct client* const client = arg;
    char const * const state_name = rawrtc_signaling_state_to_name(state);
    ZF_LOGD("(%s) Signaling state change: %s\n", client->name, state_name);
}

// Stop the main loop.
void default_signal_handler(int sig) {
    ZF_LOGI("Got signal: %d, terminating...\n", sig);
    re_cancel();
}
