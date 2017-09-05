#!/bin/bash

#Script to automate building of the WebRTCDemonstrator robot software and optionally copy the result to the robot.
mkdir build
cd build
cmake
make
