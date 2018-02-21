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

### Access

After you have completed the steps above you can log into ev3dev via ssh with the username `robot` and the password `maker`.

You can also setup an NFS share by following [this guide](http://www.ev3dev.org/docs/tutorials/setting-up-an-nfs-file-share/).

# Building

The robot software is built inside a docker container, which can be downloaded via `docker pull git.fh-muenster.de:4567/ja753563/webrtcdemonstrator/ev3wrtc:latest`. 
In order to build, please execute the following:

``` shell
    cd robot
    ../container/run_ev3wrtc.sh
    mkdir -p build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../bin ..
    make install
```

Feel free to replace the `-DCMAKE_INSTALL_PREFIX=<target_dir>` with whatever directory works for you.

**Note:** In order for the binary the run, the target system must have at least OpenSSL version 1.0.3. On *ev3dev*, this means installing OpenSSL from the `jessie-backports` repository.

