# EV3 #

Robot demonstrator for WebRTC datachannels.

## Goals ##

  * Get [RAWRTC](https://github.com/rawrtc/rawrtc) to compile and run on Lego Mindstorms EV3 Controller.
  * Control Lego Mindstorms Robot through a WebApp using WebRTC.
  * Show off how cool that is by building a neat demonstrator.

## Setup ##

This project requires:

  * A [LEGO MINDSTORMS Education EV3 Core Set](https://education.lego.com/en-us/shop/mindstorms%20ev3)
  * A µSD-Card of at least 4GB
  * A compatible USB-WiFi adapter

### Robot ###

1. Download the latest `ev3dev-jessie-ev3-generic-<date>.zip` file from the [ev3dev release page](https://github.com/ev3dev/ev3dev/releases).
1. Unzip the contained `ev3dev-jessie-ev3-generic-<date>.img` file.
1. Insert an empty but formatted µSD-Card into your computer. Do not mount it.
1. Run `sudo dd bs=4M if=ev3dev-jessie-ev3-generic-<date>.img of=/dev/<your µSD-Card>`.
1. Run `sync`, then remove the µSD-Card.
1. Insert the µSD-Card into your EV3 brick and start it. **Note:** The first boot may take a few minutes.
1. Insert your USB-WiFi adapter into the EV3 brick. 
1. Use the EV3 bricks buttons to navigate to `Wireless and Networks -> WiFi` to power up the adapter and log into your WiFi network.

### Webserver ###

Coming soon.

## Links ##

[RAWRTC](https://github.com/rawrtc/rawrtc)
