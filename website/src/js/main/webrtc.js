'use strict';

class WebRTCPeerConnection {
    constructor(configuration = null) {
        // Set default configuration (if none provided)
        if (configuration === null) {
            configuration = {
                iceServers: [{
                    urls: 'stun:stun.services.mozilla.com',
                }],
            };
        }

        // Create peer connection and bind events
        const pc = new RTCPeerConnection(configuration);


        pc.onnegotiationneeded = async () => {
            console.log('Negotiation needed');
        };
        pc.signalingstatechange = () => {
            console.log('Signaling state:', pc.signalingState);
        };
        pc.oniceconnectionstatechange = () => {
            console.log('ICE connection state:', pc.iceConnectionState);
        };
        pc.onicegatheringstatechange = () => {
            console.log('ICE gathering state:', pc.iceGatheringState);
        };
        pc.onconnectionstatechange = () => {
            console.log('Connection state:', pc.connectionState);
        };
        pc.onicecandidate = (event) => {
            console.log('Local ICE candidate:', event.candidate);
            sendMessage(pc.localDescription);
        };
        pc.onicecandidateerror = (event) => {
            console.error('ICE candidate error:', event);
        };
        pc.ondatachannel = (event) => {
            const dc = event.channel;
            console.log('Incoming data channel:', dc.label);

            // Bind events
            this.bindDataChannelEvents(dc);
        };

        // Store configuration & signalling instance
        this.pc = pc;
        this.dcs = {};
    }

    createDataChannel(name, options = null) {
        const pc = this.pc;

        // Create data channel and bind events
        const dc = pc.createDataChannel(name, options);
        this.bindDataChannelEvents(dc);

        // Store data channel and return
        this.dcs[name] = dc;
        return dc;
    }

    bindDataChannelEvents(dc) {
        dc._name = dc.label; // Meh!
        dc.onopen = () => {
            console.log(dc._name, 'open');
        };
        dc.onclose = () => {
            console.log(dc._name, 'closed');
        };
        dc.onerror = (event) => {
            console.log(dc._name, 'error:', event);
        };
        dc.onbufferedamountlow = () => {
            console.log(dc._name, 'buffered amount low:', dc.bufferedAmount);
        };
        dc.onmessage = (event) => {
            const size = event.data.byteLength || event.data.size;
            console.log(dc._name, 'incoming message (' + size + ' bytes)');
            console.log(event.data);
        };
    }

    async handleDescription(description) {
        // Set remote description
        console.log('Setting remote description');
        await this.pc.setRemoteDescription(description);
        console.log('Remote description:', this.pc.remoteDescription);

        console.log(name, 'Creating answer');
        description = await this.pc.createAnswer();

        // Apply local description
        await this.pc.setLocalDescription(description);
        console.log('Local description:', description);
    }
}

// TESTING
//____________________________________________

// Get remoteDescriptionTextarea
const remoteDescriptionTextarea = document.getElementById('remote-description');
remoteDescriptionTextarea.value =''

// Apply remote description when pasting
function onRemoteDescriptionTextareaChange () {
    // Remove event listener
    remoteDescriptionTextarea.oninput = null;
    remoteDescriptionTextarea.onchange = null;

    // Apply remote description once (needs to include candidates)
    const description = JSON.parse(remoteDescriptionTextarea.value);
    receiveMessages(description)
};
remoteDescriptionTextarea.oninput = onRemoteDescriptionTextareaChange;
remoteDescriptionTextarea.onchange = onRemoteDescriptionTextareaChange;





document.getElementById("connectSignaling").onclick = sendingData123

// Get message size
const messageSize = 1400;

function sendingData123() {

    let data = new Uint8Array(messageSize);
    try {
        dc.send(data);
    } catch (error) {
        if (error.name === 'TypeError') {
            console.error(dc._name, 'message too large to send');
        } else {
            console.error(dc._name, 'Unknown error:', error.name);
        }
    }

}


// TESTING
//____________________________________________



// Functions

/*
handle a offer from signaling server
*/
function handleOffer(message) {
    pc.handleDescription(message, true)
    .catch((error) => console.error(error));
}

/*
send data do other peer via dataChannel
*/
function sendingData(data) {
    console.log(data);
    try {
        dc.send(data);
    } catch (error) {
        if (error.name === 'TypeError') {
            console.error(dc._name, 'message too large to send');
        } else {
            console.error(dc._name, 'Unknown error:', error.name);
        }
    }

}
