# EV3 #

Robot demonstrator for WebRTC datachannels.

## Goals ##

  * Get [RAWRTC](https://github.com/rawrtc/rawrtc) to compile and run on Lego Mindstorms EV3 Controller.
  * Control Lego Mindstorms Robot through a WebApp using WebRTC.
  * Show off how cool that is by building a neat demonstrator.
  
## Relevant Files and Folders

- `robot/`: Files needed for building the robot components of this project. See `robot/README.md`.
- `website/`: Files needed for the web components of this project. See `website/README.md`.
- `container/`: Docker container containing cross-compilaton environment.

## Links ##

[RAWRTC](https://github.com/rawrtc/rawrtc)

## Protocol

### Process

1. Set up WebRTC connection
2. Create a reliable, ordered data channel named "api"
3. After the data channel has been opened the robot sends a message containing a JSON-array of JSON-objects consisting of "port" – the number or number of the port –  and "type" - the type of sensor or motor that is attached. E.g. `{"port": "A", "type": "tacho-motor-l", //or "tacho-motor-m"}`.
4. The website send messages containing JSON-strings addressing a sensor or motor by its port. See below for details.
5. If the WebRTC connection collapses, the software on the robot restarts.

### Messages

#### Motors

```
{
  "port": "A", 
  "mode": "run-to-rel-position",
  "value": "180"
}
```

| command             | value                 | meaning                                                                                                                                                | answer                                                       |
|---------------------|-----------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------|
| stop                | %                     | Stops the motor.                                                                                                                                       | %                                                            |
| run-forever         | INT32_MIN – INT32_MAX | Run until a different command is sent. Value is rotation per minute in degrees. The maximum obtainable speed depends on the motor and battery voltage. | %                                                            |
| run-to-rel-position | INT32_MIN – INT32_MAX | Run specified number of steps. Steps per degree/rotation depend on the motor. Positive values turn clockwise and vice versa.                           | %                                                            |
| set-position        | INT32_MIN – INT32_MAX | Define the current position as <value> number of steps.                                                                                                | %                                                            |
| set-stop-action     | `hold OR coast`[^1]   | Define what the motor does after receivin a stop command.                                                                                              | %                                                            |
| get-state           | %                     | Get current state of the motor.                                                                                                                        | `running OR ramping OR holding OR overloaded OR stalled`[^2] |

#### Sensors

Messages to sensors can either set the mode of a sensor by including the `mode` and `port` attributes or - by only including the `port` attribute - solicit the value(s) of the sensor. If the value has been requested, the robot will answer with a JSON-Object containing the port and an array of one or two values. 

*Mode:*
 ```
{
"port": "A",
"mode": "US-DIST-CM"
}
```
*Value request:*
```
{
"port": "A"
}
 ```

*Value answer:*
```
{
"port": "A",
"values": [1000.0]
}
 ```
 
| type           | mode        | meaning                                            | values                                   |
|----------------|-------------|----------------------------------------------------|------------------------------------------|
| lego-ev3-us    | US-DIST-CM  | continous distance measurement                     | 0 – 2550 (mm)                        |
| lego-ev3-us    | US-SI-CM    | single distance measurement                        | 0 – 2550  (mm)                       |
| lego-ev3-touch | %           | button pressed or released                         | 0 OR 1                                   |
| lego-ev3-gyro  | GYRO-ANG    | angle the sensor has turned since initializasation | -32768 – 32767 (°)                       |
| lego-ev3-gyro  | GYRO-RATE   | rate of turning                                    | -440 – 440 (°/s)                         |
| lego-ev3-gyro  | GYRO-G&A    | angle and rate of turning                          | \[-32768 – 32767 (°), -440 – 440 (°/s)\] |
| lego-ev3-color | COL-REFLECT | intensity of reflected light                       | 0-100 (%)                                |
| lego-ev3-color | COL-AMBIENT | intensity of ambient light                         | 0-100 (%)                                |
| lego-ev3-color | COL-COLOR   | detected color                                     | 0-7[^3]                                  |

#### Meta-Devices

Meta-Devices are devices either made up of more than one sensor or motor, or adding some extra functionality to a single sensor (usually automated polling). The resulting data does not need to be requested but is sent automatically on a separate data channel called `sensors`.

All existing meta-devices are described in the first message sent on the `api` channel from the robot to the website, just like the all physical devices. Their (physically non-existent) ports are designated lower-case letters starting at `"a"`. For the possible types, see the table below.

Message to a meta device have to be sent on the `api` channel as well and have to conform to the same structure as those for other sensors:
 ```
{
"port": "a",
"mode": "start",
"value": 100
}
```
Possible values for mode are `"start"` for starting and `"stop"` for stopping a meta-device. Some meta-devices need a parameter given with the `"value"` attribute (see table below). No confirmation is returned. For the functionality of each device, see the table below. While a device is running (meaning after a start and before a stop message) it will send data on the `sensors` channel. The format of the returned data is the same as for the normal sensors.

| type           | function                                                                                                                      | parameter                                               | return value(s)             |
|----------------|-------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------|-----------------------------|
| meta-collision | Collision sensor. Sends data only when value of touch sensor at the front of the robot changes.                               | %                                                       | 0=no collision, 1=collision |
| meta-sonar     | The ultrasound distance sensor, mounted on a base that swivels 180 degrees in steps, measuring the distance after every step. | %                                                       |  \[-90 – 90(°), 0 – 2550(mm)\] |
| meta-compass   | Angle the sensor has turned around its horizontal axis since initialization.                                                  | 0-2^32 , milliseconds to wait between polling the sensor | -32768 – 32767 (°)          |


[^1]: `hold` = motor attempts to hold position; `coast` = motor is free to be turned
[^2]: `running` = motor is running; `ramping` = motor is accelerating or decelerating; `holding` = motor is trying to hold position; `overloaded` = motor can not reach specified speed due to lack of power; `stalled` = motor has stalled because of insufficient torque
[^3]: `0` = none; `1` = black; `2` = blue, `3` = green, `4` = yellow, `5` = red, `6` = white, `7` = brown
