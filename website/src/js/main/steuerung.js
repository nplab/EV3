/****************
Controlling of Cancas Elements
*****************/

var canvasSteuerung = document.getElementById('canvasSteuerung');
var canvasSteuerungContext = canvasSteuerung.getContext('2d');
var canvasAbstand = document.getElementById('canvasAbstand');
var canvasAbstandContext = canvasAbstand.getContext('2d');
var centerX = 150;
var centerY = 73;
var distance = null;
var angle = null;
var sensorAuswertung = "0";
var xy_gyro;
var breakSending = 0;
// var for paint points
var points = []
var pointNr = 0

var ALLOWSENDING = false;
var INTITIALPAGE = 0;

var button_start = document.getElementById("start_button");
var button_stop = document.getElementById("stop_button");
var select_sensorAuswertung = document.getElementById("sensorAuswertung");
select_sensorAuswertung.value = "0";

// Start des Canvas Elements
function startupCanvas() {
  var radius = 70;

  drawControlingCanvas(radius);

  // Canvas für die Darstellung der Hinternisse
  drawImpedimentCanvas(radius);
}

/**********
Event Listener
 **********/

// Canvas
window.addEventListener('load', startupCanvas, false);
canvasSteuerung.addEventListener('click', function(evt) {
    if(ALLOWSENDING == true) {
        var mousePos = getMousePos(canvasSteuerung, evt);
        angleDistance = getDistanceAngle(mousePos.x, mousePos.y);
        sendMotorManagement(angleDistance)
    }
}, false);

$(document).keyup(function(e) {
     if (e.keyCode == 27) { // escape key maps to keycode `27`
        stop()
    }
});

select_sensorAuswertung.onchange = function() {
    sensorAuswertung =  this.value; // Select for canvas
}

/************
Button
************/

button_start.onclick = function () {
    // activ Canvas Event Listener
    ALLOWSENDING = true;

    // Click on button -> select is disabled
    select_sensorAuswertung.disabled = 1;

    sendingData({"port": "a", "mode": "start"});

    switch (sensorAuswertung) {
        case "0":
            sendingData({"port": "b", "mode": "start"});
            sendingData({"port": "c", "mode": "stop"}); 
            break;
        case "1":
            sendingData({"port": "b", "mode": "stop"});
            sendingData({"port": "c", "mode": "start", "value": 5000}); // value gibt die Frequenz an
            break;
        case "2":
            sendingData({"port": "b", "mode": "start"});
            sendingData({"port": "c", "mode": "start", "value": 5000}); // value gibt die Frequenz an
            break;
        default:
            console.log("Konnte nichts Starten");
    }
}

button_stop.onclick = function () {
    // inactiv Canvas Event Listener
    ALLOWSENDING = false

    // Click on button -> select is enabled
    select_sensorAuswertung.disabled = 0;

    // inactive all sensor
    sendingData({"port": "a", "mode": "stop"});
    sendingData({"port": "b", "mode": "stop"});
    sendingData({"port": "c", "mode": "stop"});

    //stop Motor
    sendingData({"port": "A", "mode": "stop"});
    sendingData({"port": "B", "mode": "stop"});
    sendingData({"port": "C", "mode": "stop"});

}

/************
Function
************/

// controll roboter
function drawControlingCanvas(radius) {
    // Canvas für die Steuerung
    canvasSteuerungContext.beginPath();
    canvasSteuerungContext.arc(centerX, centerY, radius, 0, 2 * Math.PI, true);
    canvasSteuerungContext.fillStyle = '#f3f3f4';
    canvasSteuerungContext.fill();
    canvasSteuerungContext.lineWidth = 1;
    canvasSteuerungContext.strokeStyle = '#6f6f6f';
    canvasSteuerungContext.moveTo(80, 75 );
    canvasSteuerungContext.lineTo(220, 75);
    canvasSteuerungContext.stroke();
    canvasSteuerungContext.moveTo(150, 2 );
    canvasSteuerungContext.lineTo(150, 145);
    canvasSteuerungContext.stroke();
    canvasSteuerungContext.font = '18pt Calibri';
    canvasSteuerungContext.fillStyle = '#fbba00';
    canvasSteuerungContext.fillText("R", centerX-9, centerY+10);
    canvasSteuerungContext.font = '5pt Calibri';
    canvasSteuerungContext.fillText("|", centerX-34, centerY);
    canvasSteuerungContext.fillText("|", centerX+34, centerY);
}

// show sensor data
function drawImpedimentCanvas(radius) {
    // Canvas für die Darstellung der Hinternisse
    canvasAbstandContext.beginPath();
    canvasAbstandContext.clearRect(0, 0, canvasAbstand.width, canvasAbstand.height);
    canvasAbstandContext.arc(centerX, centerY, radius, 0, 2 * Math.PI, true);
    canvasAbstandContext.fillStyle = '#f3f3f4';
    canvasAbstandContext.fill();
    canvasAbstandContext.lineWidth = 1;
    canvasAbstandContext.strokeStyle = '#6f6f6f';
    canvasAbstandContext.moveTo(80, 75 );
    canvasAbstandContext.lineTo(220, 75);
    canvasAbstandContext.stroke();
    canvasAbstandContext.moveTo(150, 2 );
    canvasAbstandContext.lineTo(150, 145);
    canvasAbstandContext.stroke();
}

function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  var x = evt.clientX - rect.left - 345;
  var y = evt.clientY - rect.top - 150;
  console.log("x" + x);
  console.log("y" + y);

  return {
    x: x,
    y: y
  };
}

// calculate distance and angle
function getDistanceAngle(mouseX, mouseY) {
  var distance = Math.sqrt(mouseX * mouseX + mouseY * mouseY)
  var angle = 0;
  if (mouseY < 0) {
    var angle = Math.acos(mouseX/distance) * 180 / Math.PI;
  } else {
    var angle = Math.acos(-mouseX/distance) * 180 / Math.PI + 180;
  }

  // set distance = 150 to max distance/value
  if (distance > 150) {
      distance = 150
  }

 return {
      angle: Math.round(angle),
      distance: Math.round(distance),
  };
}

// send value of motors to roboter
/*
Two speeds are calculated from the distance and the angle. Depending on the direction, one motor must rotate slower than the other. This is done by a quotient.
*/

function sendMotorManagement(angleDistance) {
    var value_b;
    var value_c;
    var value_ges;

    if(angleDistance.angle > 180) {
        value_b = - 0.50;
        value_c = - 0.50;
    } else {
        value_b = calculateShare(angleDistance.angle, true);    // Werte zwischen 0 - 1
        value_c = calculateShare(angleDistance.angle);          // Werte zwischen 0 - 1
    }
    value_ges = angleDistance.distance/150 * 100;               // Werte zwischen 0 - 100;

    var message_b = {
        port: 'B',
        mode: 'run-forever',
        value: value_c * value_ges,
    }
    var message_c = {
        port: "C",
        mode: 'run-forever',
        value: value_b * value_ges,
    }

    sendingData(message_b)
    sendingData(message_c)
}

// calculate quotient
function calculateShare(number, left = false) {
    var value = 0;
    if(left == true) {
        value = Math.round((100/180 * number));
    } else {
        value = Math.round((100 - 100/180 * number));
    }
    return value/100;
}

// All incomming messages are sorted and the correspondig function were called
function handleMessages(message) {
    console.log(message);

    switch (message.port) {
        case "a":
            runIntoWall(message);
            break;
        case "b":
            handleSonar(message);
            break;
        case "c":
            handleGyroSensor(message);
            break;
        default:
            console.log("Nichts passendes gefunden!")
    }
}

// if taster send a message
function runIntoWall(msg) {
    var background = document.getElementById('background');
    if(msg.value[0] === 1) {
        background.style.background="red";
    } else {
        background.style.background="#dedede";
    }
}

function handleSonar(message) {
    savePoint(message.value)
    if (sensorAuswertung == 0) {
        drawImpedimentCanvas(70)        // radius is 70
        drawPoints(points)
    } else if (sensorAuswertung == 2) { // option = 'Beide'
        drawImpedimentCanvas(70)        // radius is 70
        drawPoints(points)
        drawPoint(xy_gyro, '#000000')
    }
}

function savePoint(value) {
    var xy = getXY(value[1], toRadiant(value[0] -90))       // 0 Angle, 1 Value
    points[pointNr++%20] = xy;
}

function toRadiant (angle) {
  return angle * (Math.PI / 180);
}

function getXY(value, angle) {
    var pointAb = value * 70/2550;
    var y = Math.sin(angle) * pointAb;
    var x = Math.cos(angle) * pointAb;

    return {x, y};
}

function drawPoints(xy) {
    for (var i = 0; i < xy.length; i++) {
        drawPoint(xy[i], '#fbba00');
    }
}

function drawPoint(xy, color) {
    canvasAbstandContext.fillStyle = color;
    canvasAbstandContext.fillRect(xy.x + 148,xy.y + 71.5,5,5);
}

function handleGyroSensor(message) {
    xy_gyro = getXY(2550, toRadiant(message.value - 90))
    if (sensorAuswertung == 1) {
        drawImpedimentCanvas(70)        // radius is 70
        drawPoint(xy_gyro, '#000000')
    } else if (sensorAuswertung == 2) { // option = 'Beide'
        drawImpedimentCanvas(70)        // radius is 70
        drawPoint(xy_gyro, '#000000')
        drawPoints(points)
    }
}

// objects are inactiv at start: first message -> activ
function handleButton(val) {
    button_start.disabled = val;
    button_stop.disabled = val;
    select_sensorAuswertung.disabled = val;
}

handleButton(1);

/************
WebRTC Connection
*************/

// Herstellen einer Peer Instanz
const pc = new WebRTCPeerConnection();

// Hestellen des DataChannels
const api_dc = pc.createDataChannel('api', {
    negotiated: true,
    id: 0,
})

const sensor_dc = pc.createDataChannel('sensors', {
    negotiated: true,
    id: 1,
})

const ping_dc = pc.createDataChannel('ping', {
    negotiated: true,
    id: 2,
})

// Messages api Data Channel
api_dc.onmessage = (event) => {
    console.log(event.data);

    if (INTITIALPAGE == 0) {
        handleButton(0);
        INTITIALPAGE = 1
    } else {
        try {
            handleMessages(JSON.parse(event.data));
        } catch (e) {
            console.error("Es wurde kein JSON Object geschickt!");
        };
    }
};

// Messages sensor Data Channel
sensor_dc.onmessage = (event) => {
    msg = JSON.parse(event.data);
    if ( typeof sensor_dc.onmessage.idx == 'undefined' ) {//if this is the first time this function is called, create "static" variable
        sensor_dc.onmessage.idx = msg.idx;
    } else {
        if (msg.idx <= sensor_dc.onmessage.idx){ //if this message is older than the newest one, ignore it
            console.log("Discarded old sensor message.")
            return;
        } else { //else, remember it as the new news
            sensor_dc.onmessage.idx = msg.idx;
        }
    }
    
    handleMessages(msg);
}

ping_dc.onopen = (event) => {
    console.log("ping open")
    setInterval(function(){
        if(ping_dc.readyState == "open"){
            ping_dc.send("Ping");
        }
    }, 1000);
}
