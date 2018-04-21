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
4. Die Website sendet JSON-codierte Nachrichten an den Roboter die mind. einen Port enthalten. Näheres siehe unten.
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
| set-stop-action     | `hold OR coast`       | Lege fest was der Motor nach dem Stop-Kommando tut.                                                                                                         | %                                                        |
| get-state           | %                     | Frage aktuellen Status des Motors ab.                                                                                                                       | `running OR ramping OR holding OR overloaded OR stalled`[^](`running` = Motor dreht; `ramping` = Motor beschleunigt oder bremst; `holding` = Motor versucht aktiv die Position zu halten; `olverloaded` = Motor kann trotz maximaler Leistung die eingestellete Geschwindigkeit nicht erreich; `stalled` = Motor steht, da das Drehmoment nicht ausreicht) |


### Metadatenformat

JSON-Array von Objekten, welche die Ports beschreiben. 

#### Beispiel Motor

Livedaten-Befehl:

```
{
  "port": "A",
  "command": "run-forever",
  "value": 80 //-100 - +100
}

{
  "port": "A", 
  "command": "run-to-rel-position",
  "value": "180"
}
```

#### Beispiel Ultraschallsensor

Metadaten:

```
{
  "port": "1",
  "type": "lego-ev3-us" //driver name
}
```

Livedaten Abfrage:

```
{ //einmal, zur Initialisierung
  "port": "1",
  "mode": ""US-DIST-CM"
}
{ //Abfrage
  "port": "1"
}
```

Livedaten Antwort:

```
{
  "port": "1",
  "value": [1000] //alle values die von diesem Sensor in diesem Modus zur Verfügung gestellt werden
}
```
