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
1. Erzeuge einen Data Channel, reliable und ordered
1. Roboter sendet Beschreibung der Geräte über DC
1. Website sendet Werte bzw. Abfragen an Roboter, dieser antwortet bei Abfragen, nicht aber bei Werten. Eine Zuordnung von Antworten zu Abfragen findet nur durch Reihenfolge statt. 
1. Bei Abbruch der WebRTC-Verbindung reinitialisiert sich das Program

### Metadatenformat

JSON-Array von Objekten, welche die Ports beschreiben. 

#### Beispiel Motor

Metadaten:
``` { 
  "port": "A", //Ausgabe des Address-Commands
  "type": "tacho-motor-l", //alternativ tacho-motor-m
}```

Livedaten-Befehl:
```{
  "port": "A",
  "command": "run-forever",
  "value": 80 //-100 - +100
}

{
  "port": "A", 
  "command": "run-to-rel-position",
  "value": "180"
}```

#### Beispiel Ultraschallsensor

Metadaten:
```{
  "port": "1",
  "type": "lego-ev3-us" //driver name
}```

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
```{
  "port": "1",
  "value": [1000] //alle values die von diesem Sensor in diesem Modus zur Verfügung gestellt werden
}```
