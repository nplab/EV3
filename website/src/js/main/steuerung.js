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

var ALLOWSENDING = false;
var TASTSENSORAKTIV = false;

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
canvasSteuerung.addEventListener('mousemove', function(evt) {
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

// Button
document.getElementById('start_button').onclick = start
document.getElementById('stop_button').onclick = stop



// Testcase - Canvas Abstand

canvasAbstand.onmousemove = function(event) {
    // var x = event.clientX - canvasAbstand.offsetLeft;
    // var y = event.clientY - canvasAbstand.offsetTop;
    // var context = canvasAbstand.getContext('2d');
    //
    // console.log("x", x);
    // console.log(y);
    //
    //
    // context.fillStyle = '#fbba00';
    // context.fillRect(x,y,10,10);
    //
    // // Mitte
    // context.fillRect(145,70,10,10);
    //
    // // Oben und Unten
    // context.fillRect(145,0,10,10);
    // context.fillRect(145,140,10,10);
    //
    // // Links und rechts
    // context.fillRect(75,70,10,10);
    // context.fillRect(215,70,10,10);

    drawpoint(0, 45)
    console.log("0-----");
    drawpoint(50, 45)
    console.log("-----");
    drawpoint(100, 45)
    console.log("-----");
    drawpoint(150, 45)
    console.log("-----");
    drawpoint(200, 45)


}



/************
Functions
************/

function start () {
    // activ Canvas Event Listener
    ALLOWSENDING = true;

    // activ all sensor
    sendingData({"port": "a", "mode": "start"});
    sendingData({"port": "b", "mode": "start"});
    sendingData({"port": "c", "mode": "start", "value": 100}); // value gibt die Frequenz an

    // var message = {
    //     value: "tastsensor",
    // }
    //
    // handleMessages(message)
}
function stop () {
    // inactiv Canvas Event Listener
    ALLOWSENDING = false

    // inactive all sensor
    sendingData({"port": "a", "mode": "stop"});
    sendingData({"port": "b", "mode": "stop"});
    sendingData({"port": "c", "mode": "stop"});

    //stop Motor
    sendingData({"port": "A", "mode": "stop"});
    sendingData({"port": "B", "mode": "stop"});
    sendingData({"port": "C", "mode": "stop"});

}

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
    canvasSteuerungContext.fillText("R", centerX-10, centerY+10);
    canvasSteuerungContext.font = '5pt Calibri';
    canvasSteuerungContext.fillText("|", centerX-34, centerY);
    canvasSteuerungContext.fillText("|", centerX+34, centerY);
}

function drawImpedimentCanvas(radius, angle = null, distance = null) {
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

  // set distance = 150 to max distance/value
  if (distance > 150) {
      distance = 150
  }

 return {
      angle: Math.round(angle),
      distance: Math.round(distance),
  };
}

//
function calculateShare(number, left = false) {
    var value = 0;
    if(left == true) {
        value = Math.round((100/180 * number));
    } else {
        value = Math.round((100 - 100/180 * number));
    }
    return value/100;
}

//
function sendMotorManagement(angleDistance) {
    var value_b;
    var value_c;
    var value_ges;
    var message_b;
    var message_c;

    if(angleDistance.angle > 180) {
        value_b = - 0.50;
        value_c = - 0.50;
    } else {
        value_b = calculateShare(angleDistance.angle, true);          // Werte zwischen 0 - 1
        value_c = calculateShare(angleDistance.angle);         // Werte zwischen 0 - 1
    }
    value_ges = angleDistance.distance/150 * 100;          // Werte zwischen 0 - 100;

    // // Debug
    // console.log('Linker Motor: ' + value_b);
    // console.log('Rechter Motor: ' + value_c);
    // console.log('Geschwindigkeit: ' + value_ges);

    message_b = {
        port: 'B',
        mode: 'run-forever',
        value: value_b * value_ges,
    }
    message_c = {
        port: "C",
        mode: 'run-forever',
        value: value_c * value_ges,
    }

    sendingData(message_b)
    sendingData(message_c)
}

// Zeichnen
function drawpoint(value, angle) {
    var pointAb = value * 70/250;

    var y = Math.sin(angle) * pointAb;
    console.log(Math.sin(y));
    var x = Math.sqrt(pointAb * pointAb - y * y);

    var context = canvasAbstand.getContext('2d');

    console.log("x", x);
    console.log(y);


    context.fillStyle = '#fbba00';
    context.fillRect(x + 140,y + 70,10,10);


}


// Wenn der Stoßsensor aktiv wird
function runIntoWall() {
    var oldhtml = document.getElementById('background');
    if(TASTSENSORAKTIV == true) {
        oldhtml.style.background="red";
        TASTSENSORAKTIV = false;
    } else {
        oldhtml.style.background="#dedede";
        TASTSENSORAKTIV = true;
    }
}

function handleMessages(message) {
    console.log(message);

    switch (message.value) {
        case "tastsensor":
            runIntoWall();
            break;
        default:
            print("Nichts passendes gefunden!")
    }
}

// // Sending data to roboter
// function sendingToRoboter(mode = null, direction = null) {
//     var message = null;
//     if (direction != null) {
//         message = {
//             'port': port,
//             'mode': mode,
//             'value': direction,
//
//         }
//     } else if (mode != null) {
//         message = {
//             'port': port,
//             'mode': mode,
//         }
//     } else {
//         message = {
//             'port': port,
//         }
//     }
//
//     sendingData(message)
// }

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
    try {
        handleMessages(JSON.parse(event.data));
    } catch (e) {
        console.error("Es wurde kein JSON Object geschickt!");
    };
}
