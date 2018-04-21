class Motor {
    constructor(port, type) {
        this.port = port;
        this.type = type;
        this.modes = this.getModes();
        this.selectedMode = null;
        this.value = null;
    }
    getModes() {
        var mode1 = 'run-forever';
        var mode2 = 'run-to-rel-position';

        return [mode1, mode2]
    }
    setSelectedMode(mode) {
        this.selectedMode = mode;
    }
    setValue(value) {
        if (this.checkValue(value)) {
            this.value = value
            return true
        }
        return false
    }
    checkValue(value) {
        if (this.selectedMode == "run-forever") {
            if(value <= 100 && value >= 0) {
                return true
            }
            alert("Der Wert muss eine Zahl zwischen 0 und 100 sein.")
            return false
        } else  if (this.selectedMode = 'run-to-rel-position'){
            if(value <= 360 && value >= 0) {
                return true
            }
            alert("Der Wert muss eine Zahl zwischen 0 und 360 sein.")
            return false
        }
    }
    drive(direction) {
        sendingToRoboter(this.port, this.selectedMode, this.value * direction);
    }
    stop() {
        sendingToRoboter(this.port)
    }

}

class Sensor {
    constructor(port, type) {
        this.port = port;
        this.type = type;
        this.modes = this.getModes();
        this.selectedMode = null;
        this.value = null;
    }
    getModes() {
        switch (this.type) {
            case 'lego-ev3-us':
                var mode1 = 'US-DIST-CM'
                var mode2 = 'US-SI-CM'

                this.modes = [mode1, mode2]
                break;
            case 'lego-ev3-gyro':
                var mode1 = 'GYRO-ANG'
                var mode2 = 'GYRO-RATE'
                var mode3 = 'GYRO-G&A'

                this.modes = [mode1, mode2, mode3]
                break;
            case 'lego-ev3-color':
                var mode1 = 'COL-REFLECT'
                var mode2 = 'COL-AMBIENT'
                var mode3 = 'COL-COLOR'

                this.modes = [mode1, mode2, mode3]
                break;
            case 'lego-ev3-touch':
                var mode1 = 'TOUCH'

                this.modes = [mode1]
                break;

            default:
                console.error("Es wurde kein passender Typ definiert!");
        }
    }
    setMode() {
        sendingToRoboter(this.port, this.selectedMode);
    }
    getData() {
        sendingToRoboter(this.port)
    }
    stop() {
        sendingToRoboter(this.port, 'stop')
    }
}



var buttonVW_PortA = document.getElementById("button_portA_vw")
var buttonRW_PortA = document.getElementById("button_portA_rw")
var buttonVW_PortB = document.getElementById("button_portB_vw")
var buttonRW_PortB = document.getElementById("button_portB_rw")
var buttonVW_PortC = document.getElementById("button_portC_vw")
var buttonRW_PortC = document.getElementById("button_portC_rw")
var buttonVW_PortD = document.getElementById("button_portD_vw")
var buttonRW_PortD = document.getElementById("button_portD_rw")

var mode_PortA = document.getElementById("modeportA")
var mode_PortB = document.getElementById("modeportB")
var mode_PortC = document.getElementById("modeportC")
var mode_PortD = document.getElementById("modeportD")

var value_PortA = document.getElementById("valueportA")
var value_PortB = document.getElementById("valueportB")
var value_PortC = document.getElementById("valueportC")
var value_PortD = document.getElementById("valueportD")

var buttonSM_Port1 = document.getElementById("button_port1_sm")
var buttonSM_Port2 = document.getElementById("button_port2_sm")
var buttonSM_Port3 = document.getElementById("button_port3_sm")
var buttonSM_Port4 = document.getElementById("button_port4_sm")

var buttonGD_Port1 = document.getElementById("button_port1_get")
var buttonGD_Port2 = document.getElementById("button_port2_gd")
var buttonGD_Port3 = document.getElementById("button_port3_gd")
var buttonGD_Port4 = document.getElementById("button_port4_gd")

var buttonST_PortA = document.getElementById("button_portA_stop")
var buttonST_PortB = document.getElementById("button_portB_stop")
var buttonST_PortC = document.getElementById("button_portC_stop")
var buttonST_PortD = document.getElementById("button_portD_stop")

var mode_Port1 = document.getElementById("modeport1")
var mode_Port2 = document.getElementById("modeport2")
var mode_Port3 = document.getElementById("modeport3")
var mode_Port4 = document.getElementById("modeport4")

var value_Port1 = document.getElementById("valueport1")
var value_Port2 = document.getElementById("valueport2")
var value_Port3 = document.getElementById("valueport3")
var value_Port4 = document.getElementById("valueport4")

var motorA = null;
var motorB = null;
var motorC = null;
var motorD = null;

var sensor1 = null;
var sensor2 = null;
var sensor3 = null;
var sensor4 = null;


buttonST_PortA.onclick = function () {
    motorA.stop()
}
buttonST_PortB.onclick = function () {
    motorB.stop()
}
buttonST_PortC.onclick = function () {
    motorC.stop()
}
buttonST_PortD.onclick = function () {
    motorD.stop()
}


mode_PortA.onchange = function() {
      motorA.selectedMode = $("#modeportA :selected").text();
      value_PortA.disabled = 0
      value_PortA.value = ''
}
mode_PortB.onchange = function() {
      motorB.selectedMode = $("#modeportB :selected").text();
      value_PortB.disabled = 0
      value_PortA.value = ''
}
mode_PortC.onchange = function() {
      motorC.selectedMode = $("#modeportC :selected").text();
      value_PortC.disabled = 0
      value_PortA.value = ''
}
mode_PortD.onchange = function() {
      motorD.selectedMode = $("#modeportD :selected").text();
      value_PortD.disabled = 0
      value_PortA.value = ''
}

value_PortA.onchange = function () {
    if(motorA.setValue(parseInt(value_PortA.value))) {
        buttonVW_PortA.disabled = 0
        buttonRW_PortA.disabled = 0
    } else {
        buttonVW_PortA.disabled = 1
        buttonRW_PortA.disabled = 1
    }
}
value_PortB.onchange = function () {
    if(motorB.setValue(parseInt(value_PortB.value))) {
        buttonVW_PortB.disabled = 0
        buttonRW_PortB.disabled = 0
    } else {
        buttonVW_PortB.disabled = 1
        buttonRW_PortB.disabled = 1
    }
}
value_PortC.onchange = function () {
    if(motorC.setValue(parseInt(value_PortC.value))) {
        buttonVW_PortC.disabled = 0
        buttonRW_PortC.disabled = 0
    } else {
        buttonVW_PortC.disabled = 1
        buttonRW_PortC.disabled = 1
    }
}
value_PortD.onchange = function () {
    if(motorD.setValue(parseInt(value_PortD.value))) {
        buttonVW_PortD.disabled = 0
        buttonRW_PortD.disabled = 0
    } else {
        buttonVW_PortD.disabled = 1
        buttonRW_PortD.disabled = 1
    }
}

document.getElementById("onstart").onclick = start


function start() {


    var input = [{
        "port": "A", //Ausgabe des Address-Commands
        "type": "tacho-motor-l", //alternativ tacho-motor-m
    },
    {
        "port": "B", //Ausgabe des Address-Commands
        "type": "tacho-motor-l", //alternativ tacho-motor-m
    },
    {
        "port": "C", //Ausgabe des Address-Commands
        "type": "tacho-motor-l", //alternativ tacho-motor-m
    },
    {
        "port": "1", //Ausgabe des Address-Commands
        "type": "lego-ev3-us", //alternativ tacho-motor-m
    }
    ]


    // getDatafromRoboter(input);



    var motorBeispiel = new Motor("A", 'motor');
    var sensorBeispiel = new Sensor("1", 'lego-ev3-us')

    console.log(motorBeispiel);
    console.log(sensorBeispiel);
}

function getDatafromRoboter(input) {
    for (var i = 0; i < input.length; i++) {
        createInstanz(input[i])
    }
}

function createInstanz(input) {
    switch (input.port) {
        case "A":
            motorA = new Motor(input.port, input.type)
            handleModes(motorA, mode_PortA);
            document.getElementById("StatusportA").firstChild.data = "Connected: " + input.type
            buttonST_PortA.disabled = 0
            break;
        case "B":
            motorB = new Motor(input.port, input.type)
            handleModes(motorB, mode_PortB);
            document.getElementById("StatusportB").firstChild.data = "Connected: " + input.type
            buttonST_PortB.disabled = 0
            break;
        case "C":
            motorC = new Motor(input.port, input.type)
            handleModes(motorC, mode_PortC);
            document.getElementById("StatusportC").firstChild.data = "Connected: " + input.type
            buttonST_PortC.disabled = 0
            break;
        case "D":
            motorD = new Motor(input.port, input.type)
            handleModes(motorD, mode_PortD);
            document.getElementById("StatusportD").firstChild.data = "Connected: " + input.type
            buttonST_PortD.disabled = 0
            break;
        case "1":
            sensor1 = new Sensor(input.port, input.type)
            buttonSM_Port1.disabled = 0
            handleModes(sensor1, mode_Port1)
            mode_Port1.disabled = 0
            break;
        case "2":
            sensor2 = new Sensor(input.port, input.type)
            break;
        case "3":
            sensor3 = new Sensor(input.port, input.type)
            break;
        case "4":
            sensor4 = new Sensor(input.port, input.type)
            break;
        default:
            console.log("Kein Port für Motor- oder Sensor gefunden.");
    }
}

// ON Start
disabledAllMotorButton(1)

// Forward
buttonVW_PortA.onclick = function() {
    motorA.drive(1);
}
buttonVW_PortB.onclick = function() {
    motorB.drive(1);
}
buttonVW_PortC.onclick = function() {
    motorC.drive(1);
}
buttonVW_PortD.onclick = function() {
    motorD.drive(1);
}

// Backward
buttonRW_PortA.onclick = function() {
    motorA.drive(-1);
}
buttonRW_PortB.onclick = function() {
    motorB.drive(-1);
}
buttonRW_PortC.onclick = function() {
    motorC.drive(-1);
}
buttonRW_PortD.onclick = function() {
    motorD.drive(-1);
}

// Set Mode
buttonSM_Port1.onclick = function() {
    setMode("1");
}
buttonSM_Port2.onclick = function() {
    setMode("2");
}
buttonSM_Port3.onclick = function() {
    setMode("3");
}
buttonSM_Port4.onclick = function() {
    setMode("4");
}

// Get Data
buttonGD_Port1.onclick = function() {
    getData("1");
}
buttonGD_Port2.onclick = function() {
    getData("2");
}
buttonGD_Port3.onclick = function() {
    getData("3");
}
buttonGD_Port4.onclick = function() {
    getData("4");
}



// Sending data to roboter
function sendingToRoboter(port, mode = null, direction = null) {
    console.log(port, direction, mode);
}



// function setMode(port) {
//     var mode = getMode(port)
//     sendingToRoboter(port, mode)
// }
//
// function getData(port) {
//     sendingToRoboter(port);
// }

function handleModes(motor, modeMotor) {
    for( var i = 0; i < motor.modes.length; i++) {
        var option = document.createElement('option');
        option.text = motor.modes[i]
        option.value = i;
        modeMotor.options[i+1] = option
    }
    modeMotor.disabled = 0

}

// function handleType(input) {
//     switch (input.type) {
//         case "lego-ev3-us":
//             var option = document.createElement('option');
//             option.text = 'US-DIST-CM';
//             option.value = 1;
//             modeport1.options[0] = option
//             break;
//         case "lego-ev3-gyro":
//             var option = document.createElement('option');
//             option.text = 'US-DIST-CM';
//             option.value = 1;
//             modeport1.options[0] = option
//             break;
//         default:
//             console.error("Keinen passenden Typen gefunden!");
//     }
// }


function disabledAllMotorButton(yesno) {
    buttonVW_PortA.disabled = yesno
    buttonRW_PortA.disabled = yesno
    buttonVW_PortB.disabled = yesno
    buttonRW_PortB.disabled = yesno
    buttonVW_PortC.disabled = yesno
    buttonRW_PortC.disabled = yesno
    buttonVW_PortD.disabled = yesno
    buttonRW_PortD.disabled = yesno


    mode_PortA.disabled = yesno
    mode_PortB.disabled = yesno
    mode_PortC.disabled = yesno
    mode_PortD.disabled = yesno

    mode_PortA.value = 0
    mode_PortB.value = 0
    mode_PortC.value = 0
    mode_PortD.value = 0

    value_PortA.disabled = yesno
    value_PortB.disabled = yesno
    value_PortC.disabled = yesno
    value_PortD.disabled = yesno

    value_PortA.value = ""
    value_PortB.value = ""
    value_PortC.value = ""
    value_PortD.value = ""

    buttonSM_Port1.disabled = yesno
    buttonSM_Port2.disabled = yesno
    buttonSM_Port3.disabled = yesno
    buttonSM_Port4.disabled = yesno

    buttonGD_Port1.disabled = yesno
    buttonGD_Port2.disabled = yesno
    buttonGD_Port3.disabled = yesno
    buttonGD_Port4.disabled = yesno

    mode_Port1.disabled = yesno
    mode_Port2.disabled = yesno
    mode_Port3.disabled = yesno
    mode_Port4.disabled = yesno

    value_Port1.disabled = yesno
    value_Port2.disabled = yesno
    value_Port3.disabled = yesno
    value_Port4.disabled = yesno

    buttonST_PortA.disabled = yesno
    buttonST_PortB.disabled = yesno
    buttonST_PortC.disabled = yesno
    buttonST_PortD.disabled = yesno

}
