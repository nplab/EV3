/**********
Variablen
 **********/
var canvas = document.getElementById('canvasSteuerung');
var canvasContext = canvas.getContext('2d');
var centerX = 150;
var centerY = 73;

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
  canvasContext.stroke();
  canvasContext.font = '18pt Calibri';
  canvasContext.fillStyle = '#fbba00';
  canvasContext.fillText("R", centerX-11, centerY+6)
}


function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  var x = evt.clientX - rect.left - 267;
  var y = evt.clientY - rect.top - 128;
  // console.log(x);
  // console.log(y);
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
}, false);
