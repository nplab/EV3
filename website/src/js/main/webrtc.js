'use strict';
/*
In dieser Klasse wid eine WebRTC Verbindung erzeugt. Sie kann mittls 'const pc = new WebRTCPeerConnection()' hergestellt werden. In dieser Klasse sind schon alle notwendigen Funktionen definiert, sodass nur die Funktion 'handleOffer' aufgerufen werden muss. Anschließend wird eine remoteDescription erzeugt und an den Signaling Server gesendet.
*/
class WebRTCPeerConnection {
    constructor(configuration = null) {
        // Setzen einer default Konfiguration
        if (configuration === null) {
            configuration = {
                iceServers: [{
                    urls: 'stun:stun.services.mozilla.com',
                }],
            };
        }

        // Erzeugen einer RTCPeerConnection
        const pc = new RTCPeerConnection(configuration);

        // Initialisierung der verschiedenen "Change" Funktionen der RTC-Peers
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
        pc.onicecandidate = function(event) {
            sendMessage({ "type": "candidate", "sdp": event.candidate});
            console.log('Local ICE candidate:', event.candidate);
            // sendMessage(pc.localDescription);
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

    // Herstellen eines DataChannels
    createDataChannel(name, options = null) {
        const pc = this.pc;

        // Create data channel and bind events
        const dc = pc.createDataChannel(name, options);
        this.bindDataChannelEvents(dc);

        // Store data channel and return
        this.dcs[name] = dc;
        return dc;
    }

    // handle der Events des DataChannels
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

    // Handle description:
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

    async handleRemoteICECandidate(sdp_string){
        await this.pc.addIceCandidate(new RTCIceCandidate(sdp_string));
    }
}

// Handle einer Offer-Nachricht
function handleOffer(message) {
    pc.handleDescription(message, true)
    .catch((error) => console.error(error));
}


// Senden des Nachrichten in den DataChannel
function sendingData(data) {
    console.log(data);
    try {
        dc.send(JSON.stringify(data));
    } catch (error) {
        if (error.name === 'TypeError') {
            console.error(dc._name, 'message too large to send');
        } else {
            console.error(dc._name, 'Unknown error:', error.name);
        }
    }

}
