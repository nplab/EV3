FROM ev3dev/debian-jessie-cross

ENV CC=arm-linux-gnueabi-gcc
ENV CXX=arm-linux-gnueabi-g++
ENV PKG_CONFIG_PATH=/usr/lib/arm-linux-gnueabi/pkgconfig:/home/compiler/rawrtc/build/prefix/lib/pkgconfig

WORKDIR /home/compiler

RUN sudo echo "deb http://ftp.debian.org/debian jessie-backports main" | sudo tee -a /etc/apt/sources.list
RUN sudo apt-get update
RUN sudo apt-get -y install git curl 
RUN sudo apt-get -y -t jessie-backports install libssl-dev:armel cmake
RUN mkdir libs
RUN git clone -b crc32c_arm https://github.com/JesApp/rawrtc.git
WORKDIR rawrtc
RUN ./make-dependencies.sh
RUN cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/build/prefix -DCMAKE_TOOLCHAIN_FILE=/home/compiler/rawrtc/gcc-arm.cmake .
RUN make install
WORKDIR /home/compiler
ENV PKG_CONFIG_PATH=/home/compiler/rawrtc/lib/pkgconfig:/usr/lib/arm-linux-gnueabi/pkgconfig
