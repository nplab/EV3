class Motor {
    constructor(port, type, mode = null) {
        this.port = port;
        this.type = type;
        this.modes = mode;
        this.selectedMode = null;
        this.value = null;

        // Set List of modes
        this.getModes()
    }
    getModes() {
        var mode1 = 'run-forever';
        var mode2 = 'run-to-rel-position';

        this.modes = [mode1, mode2]
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
        sendingToRoboter(this.port, 'stop')
    }

}
