// Testcase

// Test Anwednung - Erste Nachricht
document.getElementById("onstart").onclick = testMetaDevices


function testMetaDevices() {
    // sendingToRoboter("a", "start");
    // setTimeout(function(){
    //     sendingToRoboter("a", "stop");
    // }, 10000);
    // sendingToRoboter("b", "start");
    // setTimeout(function(){
    //     sendingToRoboter("b", "stop");
    // }, 10000);
    sendingData({"port": "c", "mode": "start", "value": 100}); // 100 = Polling Frequenz
    setTimeout(function(){
        sendingToRoboter("c", "stop");
    }, 10000);
}



// Test DataChannel

document.getElementById("SetModeTest").onclick = SetModeTest
document.getElementById("StartTest").onclick = StartTest
document.getElementById("StopTest").onclick = StopTest

var TestPortA = new Motor('A', 'tacho-motor-l', null, true)
var TestPortB = new Motor('B', 'tacho-motor-l', null, true)

function SetModeTest() {
    // TestPortB.setPosition(10)
    // sendingToRoboter('A', 'run-forever', 80)

    handleMessages({port: "A", values: 'dasdasd'})
    handleMessages({port: "B", values: 'dasdasd43214'})
    handleMessages({port: "D", values: 'da31232sdasd'})
    handleMessages({port: "C", values: '123dasdasd'})
}

function StartTest() {
    TestPortA.getState()
    TestPortB.getState()
}

function StopTest() {
    TestPortA.setStopAction('hold')
    TestPortB.setStopAction('coast')
}

// Variablen für die Oberflächenaktionen
var buttonVW_PortA = document.getElementById("button_portA_vw")
var buttonRW_PortA = document.getElementById("button_portA_rw")
var buttonVW_PortB = document.getElementById("button_portB_vw")
var buttonRW_PortB = document.getElementById("button_portB_rw")
var buttonVW_PortC = document.getElementById("button_portC_vw")
var buttonRW_PortC = document.getElementById("button_portC_rw")
var buttonVW_PortD = document.getElementById("button_portD_vw")
var buttonRW_PortD = document.getElementById("button_portD_rw")

var buttonST_PortA = document.getElementById("button_portA_stop")
var buttonST_PortB = document.getElementById("button_portB_stop")
var buttonST_PortC = document.getElementById("button_portC_stop")
var buttonST_PortD = document.getElementById("button_portD_stop")

var buttonGS_PortA = document.getElementById("button_portA_getstate")
var buttonGS_PortB = document.getElementById("button_portB_getstate")
var buttonGS_PortC = document.getElementById("button_portC_getstate")
var buttonGS_PortD = document.getElementById("button_portD_getstate")

var buttonSM_Port1 = document.getElementById("button_port1_sm")
var buttonSM_Port2 = document.getElementById("button_port2_sm")
var buttonSM_Port3 = document.getElementById("button_port3_sm")
var buttonSM_Port4 = document.getElementById("button_port4_sm")

var buttonGD_Port1 = document.getElementById("button_port1_get")
var buttonGD_Port2 = document.getElementById("button_port2_gd")
var buttonGD_Port3 = document.getElementById("button_port3_gd")
var buttonGD_Port4 = document.getElementById("button_port4_gd")

var mode_PortA = document.getElementById("modeportA")
var mode_PortB = document.getElementById("modeportB")
var mode_PortC = document.getElementById("modeportC")
var mode_PortD = document.getElementById("modeportD")

var stopmode_PortA = document.getElementById("stopmodeportA")
var stopmode_PortB = document.getElementById("stopmodeportB")
var stopmode_PortC = document.getElementById("stopmodeportC")
var stopmode_PortD = document.getElementById("stopmodeportD")

var mode_Port1 = document.getElementById("modeport1")
var mode_Port2 = document.getElementById("modeport2")
var mode_Port3 = document.getElementById("modeport3")
var mode_Port4 = document.getElementById("modeport4")

var value_PortA = document.getElementById("valueportA")
var value_PortB = document.getElementById("valueportB")
var value_PortC = document.getElementById("valueportC")
var value_PortD = document.getElementById("valueportD")

var degree_PortA = document.getElementById("degreeportA")
var degree_PortB = document.getElementById("degreeportB")
var degree_PortC = document.getElementById("degreeportC")
var degree_PortD = document.getElementById("degreeportD")

var state_PortA = document.getElementById("stateportA")
var state_PortB = document.getElementById("stateportB")
var state_PortC = document.getElementById("stateportC")
var state_PortD = document.getElementById("stateportD")

var value_Port1 = document.getElementById("valueport1")
var value_Port2 = document.getElementById("valueport2")
var value_Port3 = document.getElementById("valueport3")
var value_Port4 = document.getElementById("valueport4")

// Initialisierung der Motoren
var motorA = null;
var motorB = null;
var motorC = null;
var motorD = null;

// Initialisierung der Sensoren
var sensor1 = null;
var sensor2 = null;
var sensor3 = null;
var sensor4 = null;

// Klick Events für die Button

// Stop
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
    sensor1.setMode();
    buttonGD_Port1.disabled = 0
}
buttonSM_Port2.onclick = function() {
    sensor2.setMode();
    buttonGD_Port2.disabled = 0
}
buttonSM_Port3.onclick = function() {
    sensor3.setMode();
    buttonGD_Port3.disabled = 0
}
buttonSM_Port4.onclick = function() {
    sensor4.setMode();
    buttonGD_Port4.disabled = 0
}

// Get Data
buttonGD_Port1.onmousemove = function() {
    sensor1.getData();
}
buttonGD_Port2.onmousemove = function() {
    sensor2.getData();
}
buttonGD_Port3.onmousemove = function() {
    sensor3.getData();
}
buttonGD_Port4.onmousemove = function() {
    sensor4.getData();
}

// get State
buttonGS_PortA.onclick = function() {
    motorA.getState();
}
buttonGS_PortB.onclick = function() {
    motorB.getState();
}
buttonGS_PortC.onclick = function() {
    motorC.getState();
}
buttonGS_PortD.onclick = function() {
    motorD.getState();
}

// Onchange Events für die Mode Selections
mode_PortA.onchange = function() {
      motorA.setSelectedMode($("#modeportA :selected").text()); // Setzen des Modus
      handleMotorMode(motorA, value_PortA, degree_PortA, buttonVW_PortA, buttonRW_PortA)
}
mode_PortB.onchange = function() {
      motorB.setSelectedMode($("#modeportB :selected").text());
      handleMotorMode(motorB, value_PortB, degree_PortB, buttonVW_PortB, buttonRW_PortB)
}
mode_PortC.onchange = function() {
      motorC.setSelectedMode($("#modeportC :selected").text());
      handleMotorMode(motorC, value_PortC, degree_PortC, buttonVW_PortC, buttonRW_PortC)
}
mode_PortD.onchange = function() {
      motorD.setSelectedMode($("#modeportD :selected").text());
      handleMotorMode(motorD, value_PortD, degree_PortA, buttonVW_PortD, buttonRW_PortD)
}
mode_Port1.onchange = function() {
    sensor1.setSelectedMode($("#modeport1 :selected").text());
    handleSensorMode(buttonSM_Port1, buttonGD_Port1)
}
mode_Port2.onchange = function() {
    sensor2.setSelectedMode($("#modeport2 :selected").text());
    handleSensorMode(buttonSM_Port2, buttonGD_Port2)
}
mode_Port3.onchange = function() {
    sensor3.setSelectedMode($("#modeport3 :selected").text());
    handleSensorMode(buttonSM_Port3, buttonGD_Port3)
}
mode_Port4.onchange = function() {
    sensor4.setSelectedMode($("#modeport4 :selected").text());
    handleSensorMode(buttonSM_Port4, buttonGD_Port4)
}

stopmode_PortA.onchange = function() {
    motorA.setStopAction($("#stopmodeportA :selected").text()); // Setzen des Modus
}
stopmode_PortB.onchange = function() {
    motorB.setStopAction($("#stopmodeportB :selected").text()); // Setzen des Modus
}
stopmode_PortC.onchange = function() {
    motorC.setStopAction($("#stopmodeportC :selected").text()); // Setzen des Modus
}
stopmode_PortD.onchange = function() {
    motorD.setStopAction($("#stopmodeportD :selected").text()); // Setzen des Modus
}

// Onchange für das Value Feld der Motoren
value_PortA.onchange = function () {
    handleMotorValueField(motorA, value_PortA, buttonRW_PortA, buttonVW_PortA)
}
value_PortB.onchange = function () {
    handleMotorValueField(motorB, value_PortB, buttonRW_PortB, buttonVW_PortB)
}
value_PortC.onchange = function () {
    handleMotorValueField(motorC, value_PortC, buttonRW_PortC, buttonVW_PortC)
}
value_PortD.onchange = function () {
    handleMotorValueField(motorD, value_PortD, buttonRW_PortD, buttonVW_PortD)
}

// Onchange für das Defree Feld der Motoren
degree_PortA.onchange = function() {
    handleMotorDegree(motorA, degree_PortA)
}
// Onchange für das Defree Feld der Motoren
degree_PortB.onchange = function() {
    handleMotorDegree(motorB, degree_PortB)
}
// Onchange für das Defree Feld der Motoren
degree_PortC.onchange = function() {
    handleMotorDegree(motorC, degree_PortC)
}
// Onchange für das Defree Feld der Motoren
degree_PortD.onchange = function() {
    handleMotorDegree(motorD, degree_PortD)
}

function handleMotorDegree(motor, degreeField) {
    motor.setDegree(degreeField.value)
    console.log(degreeField.value);
}


function handleSensorMode(buttonSM, buttonGD) {
    buttonSM.disabled = 0
    buttonGD.disabled = 1
}

function handleMotorMode(motor, valuePort, degreePort, buttonVW, buttonRW) {
    valuePort.disabled = 0  // Value-Feld einblenden
    valuePort.value = ''    // Value-Feld = ''
    degreePort.value = ''
    buttonVW.disabled = 1
    buttonRW.disabled = 1
    if(motor.selectedMode == 'run-to-rel-position') {
        degreePort.disabled = 0;
    } else {
        degreePort.disabled = 1;
    }

}

function handleMotorValueField(motor, valuePort, buttonRW, buttonVW) {
    if(motor.setValue(parseInt(valuePort.value))) {
        buttonVW.disabled = 0
        buttonRW.disabled = 0
    } else {
        buttonVW.disabled = 1
        buttonRW.disabled = 1
        valuePort.value = ''
        alert("Es muss ein Wert zwischen 0 und 100 eingetragen werden!")
    }
}

// Leitet die Port und Typ Bezeichungen der angelegten Sensoren und Motoren weiter
function getDatafromRoboter(input) {
    for (var i = 0; i < input.length; i++) {
        createInstanz(input[i])
    }
}

// Initialisierung der Motor/Sensor Instanzen
function createInstanz(input) {
    switch (input.port) {
        case "A":
            motorA = new Motor(input.port, input.type)
            handleModes(motorA, mode_PortA);
            document.getElementById("StatusportA").firstChild.data = "Connected: " + input.type
            handleMotorModes(motorA, buttonST_PortA, buttonGS_PortA, stopmode_PortA)
            break;
        case "B":
            motorB = new Motor(input.port, input.type)
            handleModes(motorB, mode_PortB);
            document.getElementById("StatusportB").firstChild.data = "Connected: " + input.type
            handleMotorModes(motorB, buttonST_PortB, buttonGS_PortB, stopmode_PortB)
            break;
        case "C":
            motorC = new Motor(input.port, input.type)
            handleModes(motorC, mode_PortC);
            document.getElementById("StatusportC").firstChild.data = "Connected: " + input.type
            handleMotorModes(motorC, buttonST_PortC, buttonGS_PortC, stopmode_PortC)
            break;
        case "D":
            motorD = new Motor(input.port, input.type)
            handleModes(motorD, mode_PortD);
            document.getElementById("StatusportD").firstChild.data = "Connected: " + input.type
            handleMotorModes(motorD, buttonST_PortD, buttonGS_PortD, stopmode_PortD)
            break;
        case "1":
            sensor1 = new Sensor(input.port, input.type)
            handleModes(sensor1, mode_Port1)
            document.getElementById("Statusport1").firstChild.data = "Connected: " + input.type
            break;
        case "2":
            sensor2 = new Sensor(input.port, input.type)
            handleModes(sensor2, mode_Port2)
            document.getElementById("Statusport2").firstChild.data = "Connected: " + input.type
            break;
        case "3":
            sensor3 = new Sensor(input.port, input.type)
            handleModes(sensor3, mode_Port3)
            document.getElementById("Statusport3").firstChild.data = "Connected: " + input.type
            break;
        case "4":
            sensor4 = new Sensor(input.port, input.type)
            handleModes(sensor4, mode_Port4)
            document.getElementById("Statusport4").firstChild.data = "Connected: " + input.type
            break;
        default:
            console.log("Kein Port für Motor- oder Sensor gefunden.");
    }
}

function handleMotorModes(motor, buttonST, buttonGS, stopmode) {
    buttonST.disabled = 0
    buttonGS.disabled = 0
    stopmode.disabled = 0
    for( var i = 0; i < motor.stopAction.length; i++) {
        var option = document.createElement('option');
        option.text = motor.stopAction[i]
        option.value = i;
        stopmode.options[i+1] = option
    }
}

// Bei Nachrichteneingang wird die dem Port entsprechende Nachricht in einem Textfeld angegeben.
function handleMessages(message) {
    var data = eval(message);
    console.log(data);
    switch (data.port) {
    case "A":
        state_PortA.value = data.state;
        motorA.setState(data.state);
        break;
    case "B":
        state_PortB.value = data.state;
        motorB.setState(data.state);
        break;
    case "C":
        state_PortC.value = data.state;
        motorC.setState(data.state);
        break;
    case "D":
        state_PortD.value = data.state;
        motorD.setState(data.state);
        break;
    case '1':
        value_Port1.value = data.values;
        sensor1.setValue(data.values)
        break;
    case '2':
        value_Port2.value = data.values;
        sensor2.value = data.values;
        sensor2.setValue(data.values)
        break;
    case '3':
        value_Port3.value = data.values;
        sensor3.value = data.values;
        sensor3.setValue(data.values)
        break;
    case '4':
        value_Port4.value = data.values;
        sensor4.value = data.values;
        sensor4.setValue(data.values)
        break;
    default:
        console.error("Es wurde kein passender Port gefunden.");
    }
}

// Sending data to roboter
function sendingToRoboter(port, mode = null, direction = null) {
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

function handleModes(motor, modeMotor) {
    for( var i = 0; i < motor.modes.length; i++) {
        var option = document.createElement('option');
        option.text = motor.modes[i]
        option.value = i;
        modeMotor.options[i+1] = option
    }
    modeMotor.disabled = 0
}

// Stellt eine Grundstatus der Button und Felder her
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

    buttonGS_PortA.disabled = yesno
    buttonGS_PortB.disabled = yesno
    buttonGS_PortC.disabled = yesno
    buttonGS_PortD.disabled = yesno

    stopmode_PortA.value = 0
    stopmode_PortB.value = 0
    stopmode_PortC.value = 0
    stopmode_PortD.value = 0

    stopmode_PortA.disabled = yesno
    stopmode_PortB.disabled = yesno
    stopmode_PortC.disabled = yesno
    stopmode_PortD.disabled = yesno

    state_PortA.value = ""
    state_PortB.value = ""
    state_PortC.value = ""
    state_PortD.value = ""

    state_PortA.disabled = yesno
    state_PortB.disabled = yesno
    state_PortC.disabled = yesno
    state_PortD.disabled = yesno

    value_PortA.disabled = yesno
    value_PortB.disabled = yesno
    value_PortC.disabled = yesno
    value_PortD.disabled = yesno

    value_PortA.value = ""
    value_PortB.value = ""
    value_PortC.value = ""
    value_PortD.value = ""

    degree_PortA.disabled = yesno
    degree_PortB.disabled = yesno
    degree_PortC.disabled = yesno
    degree_PortD.disabled = yesno

    degree_PortA.value = ""
    degree_PortB.value = ""
    degree_PortC.value = ""
    degree_PortD.value = ""

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

// ON Start

disabledAllMotorButton(1)


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

// Nachrichten Eingang des DataChannels
api_dc.onmessage = (event) => {
    console.log(event.data);

    if (INTITIALPAGE == 0) {
        getDatafromRoboter(JSON.parse(event.data));
        INTITIALPAGE = 1
    } else {
        try {
            handleMessages(JSON.parse(event.data))
        } catch (e) {
            console.error("Es wurde kein JSON Object geschickt!");
        }
    }
};

sensor_dc.onmessage = (event) => {
    console.log(event.data);
}
