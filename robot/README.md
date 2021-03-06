# Robot

This folder contains:

- `src`: source files needed for building the robot components of this project.

## Setup ##

This project requires:

  * A [LEGO MINDSTORMS Education EV3 Core Set](https://education.lego.com/en-us/shop/mindstorms%20ev3)
  * A µSD-Card of at least 4GB
  * A compatible USB-WiFi adapter

In order to set up ev3dev (the operating systems for the robot), execute the following steps:

1. Download the latest `ev3dev-jessie-ev3-generic-<date>.zip` file from the [ev3dev release page](https://github.com/ev3dev/ev3dev/releases).
1. Unzip the contained `ev3dev-jessie-ev3-generic-<date>.img` file.
1. Insert an empty but formatted µSD-Card into your computer. Do not mount it.
1. Run `sudo dd bs=4M if=ev3dev-jessie-ev3-generic-<date>.img of=/dev/<your µSD-Card>`.
1. Run `sync`, then remove the µSD-Card.
1. Insert the µSD-Card into your EV3 brick and start it. **Note:** The first boot may take a few minutes.
1. Insert your USB-WiFi adapter into the EV3 brick. 
1. Use the EV3 bricks buttons to navigate to `Wireless and Networks -> WiFi` to power up the adapter and log into your WiFi network.

Also see section **Futher Setup** below.

## Access

After you have completed the steps above you can log into ev3dev via ssh with the username `robot` and the password `maker`.

You can also setup an NFS share by following [this guide](http://www.ev3dev.org/docs/tutorials/setting-up-an-nfs-file-share/).

## Construction Notes

The software expects the sensors and actors to be attached as follows:

| sensor / actuator         | port |
|---------------------------|------|
| collision switch          |    1 |
| zero switch for sonar     |    2 |
| distance sensor for sonar |    3 |
| motor for sonar           |    A |
| left-hand drive motor     |    B |
| right-hand drive motor    |    C |

# Software

## Building

The robot software is built inside a docker container, which can be downloaded via `docker pull git.fh-muenster.de:4567/ja753563/webrtcdemonstrator/ev3wrtc:latest`. 
In order to build, please execute the following:

``` shell
    cd robot
    ../container/run_ev3wrtc.sh
```
The resulting binary will be `robot/bin/wrtcrobot` and needs to be copied to the EV3 brick along with `wrtc_robot.conf` (see below).

## Further Setup

### Installation

The binary file needs to be moved from `robot/bin/wrtcrobot` to `/usr/bin/wrtcrobot` on the robot. The configuration is expected to be in `/etc/wrtcrobot/wrtcr_conf.json`.

In order for the software to be started automatically the systemd unit file `wrtcrobot.service` has been provided. It will also ensure that the software is restarted after it quits when the connection to the website has been closed. This unit file needs to be moved to `/etc/systemd/system/wrtcrobot.service` on the robot and enabled with `sudo systemctl enable wrtcrobot.service`. 

The log messages can be viewed with `sudo journalctl --system -u wrtcrobot.service`. 

### DNS
If you would like to use Hostnames instead of IP addresses in the config file, change the `host: ` line in `/etc/nsswitch.conf` to `hosts: files [NOTFOUND=continue UNAVAIL=continue FOUND=return] dns [NOTFOUND=return UNAVAIL=return TRYAGAIN=return FOUND=return] files`.

### OpenSSL
The target system must have at least OpenSSL version 1.0.3. On *ev3dev*, this means installing OpenSSL from the `jessie-backports` repository.

Add `deb http://ftp.debian.org/debian jessie-backports main` to `/etc/apt/sources.list`. Then run `apt-get -t jessie-backports install openssl`.

