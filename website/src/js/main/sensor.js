/*
In dieser Klasse sind alle notwendigen Funktionen definiert, damit ein Lego Minestrom Sensor gesteuert werden kann.

Dafür ist es notwenig, dass sich die Weboberfläche mit dem Roboter verbindet (z.B. mittels webRTC). Außerdem muss anschließend die Funktion 'sendingToRoboter' definiert werden, sodass der Roboter die Nachrichten erhalten kann.

*/
class Sensor {
    constructor(port, type, mode = null) {
        this.port = port;           // Port des Sensors
        this.type = type;           // Typ des Sensors
        this.modes = null;          // Liste der wählbaren Modes
        this.selectedMode = mode;   // Ausgewählter Modus
        this.value = null;          //

        // Set List of modes
        this.getModes()
    }
    // Je nach Typ des Sensors können unterschiedliche Modes genutzt werden.
    getModes() {
        switch (this.type) {
            case 'lego-ev3-us': // Ultraschallsensor
                var mode1 = 'US-DIST-CM'
                var mode2 = 'US-SI-CM'

                this.modes = [mode1, mode2]
                break;
            case 'lego-ev3-gyro': // Gyrosensor
                var mode1 = 'GYRO-ANG'
                var mode2 = 'GYRO-RATE'
                var mode3 = 'GYRO-G&A'

                this.modes = [mode1, mode2, mode3]
                break;
            case 'lego-ev3-color': // Farbsensor
                var mode1 = 'COL-REFLECT'
                var mode2 = 'COL-AMBIENT'
                var mode3 = 'COL-COLOR'

                this.modes = [mode1, mode2, mode3]
                break;
            case 'lego-ev3-touch':  // Tastsensor
                var mode1 = 'TOUCH'

                this.modes = [mode1]
                break;

            default:
                console.error("Es wurde kein passender Typ definiert!");
        }
    }

    // Sendet den ausgewählten Modus
    setMode() {
        sendingToRoboter(this.port, this.selectedMode);
    }
    // Sendet an den Roboter eine Nachricht. -> Erhält den Sensorwert
    getData() {
        sendingToRoboter(this.port)
    }
}
