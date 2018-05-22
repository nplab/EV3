/*
In dieser Klasse sind alle notwendigen Funktionen definiert, damit ein Lego Minestrom Motor gesteuert werden kann.

Dafür ist es notwenig, dass sich die Weboberfläche mit dem Roboter verbindet (z.B. mittels webRTC). Außerdem muss anschließend die Funktion 'sendingToRoboter' definiert werden, sodass der Roboter die Nachrichten erhalten kann.
*/

class Motor {
    constructor(port, type, mode = null, allModes = false) {
        this.port = port;               // Port des Motors
        this.type = type;               // Typ des Motors
        this.modes = null;              // Liste der wählbaren Modes
        this.selectedMode = mode;       // Ausgewählter Modus [Wird nicht überprüft]
        this.value = null;              // Geschwindigkeit
        this.degree = 100;             // Grad bei 'run-to-rel-position'
        this.state = null;              // State des Motors
        this.stopAction = null;         // Stop Action
        this.selectedStopAction = null; // Ausgewählte StopAction

        // Set List of modes
        this.getModes(allModes)
    }

    // Je nach Typ des Sensors können unterschiedliche Modes genutzt werden. Zurzeit werden für beide Motoren die beiden gleichen Modi unterstützt.
    getModes(allModes) {
        var mode1 = 'run-forever';          // läuft für immer
        var mode2 = 'run-to-rel-position';  // drehen bis zu einer bestimmten Position

        if (allModes) {
            var mode3 = 'set-position';     // definierte Position, Beispielsweise 0 Grad
            var mode4 = 'set-stop-action';  // sich drehen oder nicht
            var mode5 = 'get-state';        // Welcher Status, Läuft, usw...
            var mode6 = 'stop'              // Stop
            this.modes = [mode1, mode2, mode3, mode4, mode5, mode6]
        } else {
            this.modes = [mode1, mode2]
        }
    }

    // Setzt den selectedMode, wenn der Modus in der Modusliste existiert.
    setSelectedMode(mode) {
        if (this.modes.indexOf(mode) != -1) {
            this.selectedMode = mode;
        } else {
            console.error("Kein Passender Mode!");
        }
    }

    //
    getState() {
        sendingToRoboter(this.port, 'get-state')
    }

    //
    setState(state) {
        this.state = state;
    }

    setStopAction(action) {
        sendingToRoboter(this.port, 'set-stop-action', action)
        this.selectedStopAction = action
    }

    setPosition(position) {
        sendingToRoboter(this.port, 'set-position', position)
    }

    setDegree(degree) {
        // TODO Check Integer
        this.degree = degree
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
        if(value <= 100 && value >= 0) {
            return true
        }
        alert("Der Wert muss eine Zahl zwischen 0 und 100 sein.")
        return false
    }

    // Fahren des Roboters. Es wird eine Richtung ('direction') definiert. "+" -> Vorwärts; "-" -> Rückwärts
    drive(direction) {
        if(this.selectedMode == 'run-to-rel-position') {
            sendingToRoboter(this.port, this.selectedMode, [this.value, this.degree * direction]);
        } else {
            sendingToRoboter(this.port, this.selectedMode, this.value * direction);
        }


    }

    // Stoppen des Motors
    stop() {
        sendingToRoboter(this.port, 'stop')
    }

}
