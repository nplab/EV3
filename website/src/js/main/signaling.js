// Verbindung zum Signaling Server
var host = "https://" + wrtcr_conf.sig_serv.host + ":" + wrtcr_conf.sig_serv.port_website;
var socket = io.connect(host);

// erhalte und gebe die "info Message" aus
socket.on('info', function (data) {
    console.info(data);

    // Betrete den Raum
    socket.emit('roomJoin', wrtcr_conf.sig_serv.room);
})

// Erhalte alle Nachrichten, die im Raum gesendet werden
socket.on('signaling', function(data) {
    console.log('Client received message:', data);
    if (data === 'READY###') {
        return;
    }
    try {
        receiveMessages(JSON.parse(data))
    } catch (e) {
        console.error("Die Nachricht entspricht nicht dem JSON Format.", e);
    }
});

// Erfolgreiche Verbindung zum Raum
socket.emit('signaling', "Message to Server: Connection successfull");


// Handle Nachrichten
function receiveMessages (message) {
    if (message.type === 'offer') {
        handleOffer(message);
    } else if( message.candidate){
        handleICECandidate(message);
    }
    else if (message === 'bye') {
        // TODO, Falls eine Nachricht gesendet werden soll.
    }
}

// Senden einer Nachricht an den Siganling Server
function sendMessage(message){
    console.log('Website sending message:');
    console.log(JSON.stringify(message));
    socket.emit('signaling', JSON.stringify(message));
}
