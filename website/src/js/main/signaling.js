config = getConfig();

// Connect to Signaling Server
var socket = io.connect('https://localhost:3001');

// get and print info Message
socket.on('info', function (data) {
    console.info(data);

    // Join room
    socket.emit('roomJoin', config.sig_serv.room);
})

// get all messages from room
socket.on('signaling', function(data) {
    console.log('Client received message:', data);
    if (data === 'READY###') {
        return
    }

    receiveMessages(JSON.parse(data))
});

function receiveMessages (message) {

    if (message.type === 'offer') {
        handleOffer(message)

    } else if (message === 'bye') {
        // TODO: cleanup RTC connection?
    }
}


function sendMessage(message){
    console.log('Client sending message:');
    console.log(JSON.stringify(message));
    socket.emit('signaling', JSON.stringify(message));
}

// send message to room
socket.emit('signaling', "Message to Server: Connection successfull");
