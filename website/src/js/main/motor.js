/*
In dieser Klasse sind alle notwendigen Funktionen definiert, damit ein Lego Minestrom Motor gesteuert werden kann.

Dafür ist es notwenig, dass sich die Weboberfläche mit dem Roboter verbindet (z.B. mittels webRTC). Außerdem muss anschließend die Funktion 'sendingToRoboter' definiert werden, sodass der Roboter die Nachrichten erhalten kann.

*/

class Motor {
    constructor(port, type, mode = null) {
        this.port = port;               // Port des Motors
        this.type = type;               // Typ des Motors
        this.modes = null;              // Liste der wählbaren Modes
        this.selectedMode = mode;       // Ausgewählter Modus [Wird nicht überprüft]
        this.value = null;

        // Set List of modes
        this.getModes()
    }

    // Je nach Typ des Sensors können unterschiedliche Modes genutzt werden. Zurzeit werden für beide Motoren die beiden gleichen Modi unterstützt.
    getModes() {
        var mode1 = 'run-forever';
        var mode2 = 'run-to-rel-position';

        this.modes = [mode1, mode2]
    }

    // Setter value
    setValue(value) {
        if (this.checkValue(value)) {
            this.value = value
            return true
        }
        return false
    }

    // Der Wert muss überprüft werden, weil der Roboter in den verschiedenen Modie nur unterschiedliche Werte erlauben. Zurzeit werden nur zwei Modie unterstützt.
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

    // Fahren des Roboters. Es wird eine Richtung ('direction') definiert. "+" -> Vorwärts; "-" -> Rückwärts
    drive(direction) {
        sendingToRoboter(this.port, this.selectedMode, this.value * direction);
    }

    // Stoppen des Motors
    stop() {
        sendingToRoboter(this.port, 'stop')
    }

}
