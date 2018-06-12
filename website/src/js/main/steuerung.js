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

// Start des Canvas Elements
function startupCanvas() {
  var radius = 70;

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
  canvasSteuerungContext.fillText("R", centerX-10, centerY+10);

  // Canvas für die Darstellung der Hinternisse
  canvasAbstandContext.beginPath();
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

/**********
Event Listener
 **********/
window.addEventListener('load', startupCanvas, false);
canvasSteuerung.addEventListener('mousemove', function(evt) {
  var mousePos = getMousePos(canvasSteuerung, evt);
  angleDistance = getDistanceAngle(mousePos.x, mousePos.y);
  sendMotorManagement(angleDistance)
}, false);

// Create Motor and Sensor Instanz

var motorSteuerLinks = new Motor('A', 'tacho-motor-l', 'run-forever')

var motorSteuerRechts = new Motor('B', 'tacho-motor-l', 'run-forever')

var radarmotor = new Motor('C', 'tacho-motor-m', 'run-forever')

var sensorStoss = new Sensor('1', 'lego-ev3-touch')
var sensorRadar = new Sensor('2', 'lego-ev3-us')
var sensorStossNull = new Sensor('3', 'lego-ev3-touch')

// Testcase


// function testMetaDevices() {
//     // sendingToRoboter("a", "start");
//     // setTimeout(function(){
//     //     sendingToRoboter("a", "stop");
//     // }, 10000);
//     // sendingToRoboter("b", "start");
//     // setTimeout(function(){
//     //     sendingToRoboter("b", "stop");
//     // }, 10000);
//     sendingData({"port": "c", "mode": "start", "value": 100});
//     setTimeout(function(){
//         sendingToRoboter("c", "stop");
//     }, 10000);
// }



/************
Functions
************/

function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  var x = evt.clientX - rect.left - 337;
  var y = evt.clientY - rect.top - 162;
  // console.log("x" + x);
  // console.log("y" + y);
  // console.log(evt.clientX - rect.left);
  // console.log(evt.clientY);

  return {
    x: x,
    y: y
  };
}

function getDistanceAngle(mouseX, mouseY) {
  var distance = Math.sqrt(mouseX * mouseX + mouseY * mouseY)
  var angle = 0;
  if (mouseY < 0) {
    var angle = Math.acos(mouseX/distance) * 180 / Math.PI;
  } else {
    var angle = Math.acos(-mouseX/distance) * 180 / Math.PI + 180;
  }
  // console.log("Winkel:" + angle);
  // console.log("Abstand:" + distance);

 return {
      angle: Math.round(angle),
      distance: Math.round(distance),
  };
}

function sendMotorManagement(angleDistance) {
    console.log(angleDistance.angle);
    console.log(angleDistance.distance);
}

// Wenn der Stoßsensor aktiv wird
function runIntoWall() {
  var oldhtml = document.getElementById('background');
  oldhtml.style.background="red";
  setTimeout(function() {
    oldhtml.style.background="#dedede";
  }, 1000);
}

function handleMessages(message) {
    console.log(message);
}

// Sending data to roboter
function sendingToRoboter(mode = null, direction = null) {
    var message = null;
    if (direction != null) {
        message = {
            'port': port,
            'mode': mode,
            'value': direction,

        }
    } else if (mode != null) {
        message = {
            'port': port,
            'mode': mode,
        }
    } else {
        message = {
            'port': port,
        }
    }

    sendingData(message)
}

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
// Sorgt dafür, dass die pars-Funktion nur bei den ersten Daten einmal aufgerufen wird.
var INTITIALPAGE = 0

// Messages api Data Channel
api_dc.onmessage = (event) => {
    console.log(event.data);

    try {
        handleMessages(JSON.parse(event.data));
    } catch (e) {
        console.error("Es wurde kein JSON Object geschickt!");
    };
};

// Messages sensor Data Channel
sensor_dc.onmessage = (event) => {
    handleMessages(JSON.parse(event.data));
}
