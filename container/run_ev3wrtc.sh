#!/bin/bash

sudo docker run --rm -itv $(pwd):/home/compiler/src -w /home/compiler/src ev3wrtc:latest 
