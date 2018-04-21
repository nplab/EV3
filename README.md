# EV3 #

Robot demonstrator for WebRTC datachannels.

## Goals ##

  * Get [RAWRTC](https://github.com/rawrtc/rawrtc) to compile and run on Lego Mindstorms EV3 Controller.
  * Control Lego Mindstorms Robot through a WebApp using WebRTC.
  * Show off how cool that is by building a neat demonstrator.
  
## Relevant Files and Folders

- `robot/`: Files needed for building the robot components of this project. See `robot/README.md`.
- `website/`: Files needed for the web components of this project. See `website/README.md`.
- `container/`: Docker containers needed for building and deploying.

## Links ##

[RAWRTC](https://github.com/rawrtc/rawrtc)

## Protokoll

### Ablauf

1. Baue WebRTC Verbindung auf
2. Erzeuge einen Data Channel namens "api", reliable und ordered
3. Nach Öffnen des Data Channels sendet der Roboter über diesen einen String, welcher ein JSON-Array von JSON-Objekten enthält die aus "port" und "typ" bestehen. Bspw. `{"port": "A", "type": "tacho-motor-l", //alternativ tacho-motor-m}`.
4. Die Website sendet JSON-codierte Nachrichten an den Roboter die über den Port einen Sensor oder Motor addressieren. Näheres siehe unten.
5. Bei einem Abbruch des Data-Channels startet das Program auf dem Roboter neu.

### Nachrichten

#### Motoren

```
{
  "port": "A", 
  "command": "run-to-rel-position",
  "value": "180"
}
```

| command             | value                 | Bedeutung                                                                                                                                                   | Anwort                                                   |
|---------------------|-----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| stop                | %                     | Hält den Motor an.                                                                                                                                          | %                                                        |
| run-forever         | -100 – 100            | Drehe bis ein anderer Befehl gesendet wird. Wert ist Geschwindigkeit in Prozent. Positives Vorzeichen bedeutet eine Drehung im Uhrzeigersinn und umgekehrt. | %                                                        |
| run-to-rel-position | INT32_MIN – INT32_MAX | Drehe die angegebene Anzahl an Schritten weiter. Positives Vorzeichen bedeutet eine Drehung im Uhrzeigersinn und umgekehrt.                                 | %                                                        |
| set-position        | INT32_MIN – INT32_MAX | Setze den Wert für die aktuelle Position des Motors.                                                                                                        | %                                                        |
| set-stop-action     | `hold OR coast`[^1]      | Lege fest was der Motor nach dem Stop-Kommando tut.                                                                                                         | %                                                        |
| get-state           | %                     | Frage aktuellen Status des Motors ab.                                                                                                                       | `running OR ramping OR holding OR overloaded OR stalled`[^2] |

#### Sensoren

Nachrichten an Sensoren können entweder mittels des Atrributs `mode` einen Modus setzen oder - durch Weglassen des Attributs - die Werte abfragen. Bei Abfrage eines Wertes antwortet der Roboter mit einem JSON-Objekt das den Port sowie ein Array mit den Werten enthält.

*Modus:*
 ```
{
"port": "A",
"mode": "US-DIST-CM"
}
```
*Wert:*
```
{
"port": "A",
"values": [1000.0]
}
 ```
 
 | Sensortyp      | mode        | Bedeutung                           | Werte                                  |
 |----------------|-------------|-------------------------------------|----------------------------------------|
 | lego-ev3-us    | US-DIST-CM  | Kontinuierliche Distanzmessung      | 0.0 – 2550.0 (mm)                      |
 | lego-ev3-us    | US-SI-CM    | Einmalige Distanzmessung            | 0.0 – 2550.0  (mm)                     |
 | lego-ev3-touch | %           | Taster gedrückt oder nicht gedrückt | 0 OR 1                                 |
 | lego-ev3-gyro  | GYRO-ANG    | Drehwinkel seit Initialisierung     | -32768 – 32767 (°)                     |
 | lego-ev3-gyro  | GYRO-RATE   | Drehgeschwindigkeit                 | -440 – 440 (°/s)                       |
 | lego-ev3-gyro  | GYRO-G&A    | Drehwinkel und Drehgeschwindigkeit  | [-32768 – 32767 (°), -440 – 440 (°/s)] |
 | lego-ev3-color | COL-REFLECT | Reflektierte Lichtintensität        | 0-100 (%)                              |
 | lego-ev3-color | COL-AMBIENT | Itensität des Umgebungslichts       | 0-100 (%)                              |
 | lego-ev3-color | COL-COLOR   | Detektierte Farbe                   | 0-7[^3]                                |


[^1]: `hold` = Motor hält die letzte Position; `coast` = Motor dreht frei
[^2]: `running` = Motor dreht; `ramping` = Motor beschleunigt oder bremst; `holding` = Motor versucht aktiv die Position zu halten; `overloaded` = Motor kann trotz maximaler Leistung die eingestellete Geschwindigkeit nicht erreich; `stalled` = Motor steht, da das Drehmoment nicht ausreicht.
[^3]: `0` = keine; `1` = schwarz; `2` = blau, `3` = grün, `4` = gelb, `5` = rot, `6` = weiß, `7` = braun
