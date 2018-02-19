/**********
Variablen
 **********/
var canvas = document.getElementById('canvasSteuerung');
var canvasContext = canvas.getContext('2d');
var centerX = 150;
var centerY = 73;
var distance = null;
var angle = null;

window.onlaod = startup();


/**********
Funktionen
 **********/
function startup() {
  // Canvas
  var radius = 70;
  canvasContext.beginPath();
  canvasContext.arc(centerX, centerY, radius, 0, 2 * Math.PI, true);
  canvasContext.fillStyle = '#f3f3f4';
  canvasContext.fill();
  canvasContext.lineWidth = 1;
  canvasContext.strokeStyle = '#6f6f6f';
  canvasContext.moveTo(80, 75 );
  canvasContext.lineTo(220, 75);
  canvasContext.stroke();
  canvasContext.moveTo(150, 2 );
  canvasContext.lineTo(150, 145);
  canvasContext.stroke();
  canvasContext.font = '18pt Calibri';
  canvasContext.fillStyle = '#fbba00';
  canvasContext.fillText("R", centerX-10, centerY+10)
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
canvas.addEventListener('mousemove', function(evt) {
  var mousePos = getMousePos(canvas, evt);
  getDistanceAngle(mousePos.x, mousePos.y);
}, false);
