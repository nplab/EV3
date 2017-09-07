#!/bin/bash

sudo docker run --rm -itv $(pwd):/home/compiler/robot -w /home/compiler/robot ev3wrtc:latest 
