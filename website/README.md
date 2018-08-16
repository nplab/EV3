# Website

This folder contains:

 - `src`: source files
 - `layout`: concept of the webpage

# Setup

## Configuration

### Configure url and port of signaling server

- define in file `wrtcr_conf.js` a varibale 'wrtcr_conf'.
- Copy the contents of the configuration file `wrtcr_conf.json` and add it to the variable.

### Start signaling server

 - Starting up the [signaling server](https://github.com/nplab/WebRTC-Signaling-Server)

### Insert certificate

**Firefox**

Browsersotionen -> Privacy & Security -> Certificates -> View Certificates -> Servers -> Add Exception -> insert localhost:3001, Get Certificate -> Confirm Security Exception

**Chrome**

Open 'chrome://flags/#allow-insecure-localhost' -> Enable ,,Allow invalid certificates for resources loaded from localhost.''

## Steps

 - Switch to folder `src`
 - Run `python3 -m http.server 8001`
 - Open page: localhost:8001
 - Click on Button "Connection"



