var canvasSteuerung = document.getElementById('canvasSteuerung');
var canvasSteuerungContext = canvasSteuerung.getContext('2d');
var canvasAbstand = document.getElementById('canvasAbstand');
var canvasAbstandContext = canvasAbstand.getContext('2d');
var centerX = 150;
var centerY = 73;
var distance = null;
var angle = null;

/**********
Funktionen
 **********/
function startupCanvas() {
  var radius = 70;
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

// Wenn der Stoßsensor aktiv wird
function runIntoWall() {
  var oldhtml = document.getElementById('background');
  oldhtml.style.background="red";
  setTimeout(function() {
    oldhtml.style.background="#dedede";
  }, 1000);
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

  var data = {
      'angle': Math.round(angle),
      'distance': Math.round(distance),
  }
  sendingData(JSON.stringify(data))
}


function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  var x = evt.clientX - rect.left - 267;
  var y = evt.clientY - rect.top - 128;
  // console.log("x" + x);
  // console.log("y" + y);
  // console.log(evt.clientX - rect.left);
  // console.log(evt.clientY);

  return {
    x: x,
    y: y
  };
}


/**********
Listener
 **********/
window.addEventListener('load', startupCanvas, false);
canvasSteuerung.addEventListener('mousemove', function(evt) {
  var mousePos = getMousePos(canvasSteuerung, evt);
  getDistanceAngle(mousePos.x, mousePos.y);
}, false);

// Create Motor and Sensor Instanz

var steuermotorLinks = new Motor('A', 'tacho-motor-l')
var steuermotorRechts = new Motor('B', 'tacho-motor-l')
var radarmotor = new Motor('C', 'tacho-motor-m')

var sensorStoss = new Sensor('1', 'lego-ev3-touch')
var sensorRadar = new Sensor('2', 'lego-ev3-us')
var sensorStossNull = new Sensor('3', 'lego-ev3-touch')



// Create peer connection instance
const pc = new WebRTCPeerConnection();

// Create Data Channel
const dc = pc.createDataChannel('api', {
    negotiated: true,
    id: 0,
})
