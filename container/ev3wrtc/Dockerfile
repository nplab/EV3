FROM ev3dev/debian-jessie-cross

ENV CC=arm-linux-gnueabi-gcc
ENV PKG_CONFIG_PATH=/usr/lib/arm-linux-gnueabi/pkgconfig

WORKDIR /src

RUN sudo echo "deb http://ftp.debian.org/debian jessie-backports main" | sudo tee -a /etc/apt/sources.list
RUN sudo apt-get update
RUN sudo apt-get -y install git curl 
RUN sudo apt-get -y -t jessie-backports install libssl-dev:armel
