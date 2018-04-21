class Sensor {
    constructor(port, type, mode = null) {
        this.port = port;
        this.type = type;
        this.modes = null;
        this.selectedMode = mode;
        this.value = null;

        // Set List of modes
        this.getModes()
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
}
