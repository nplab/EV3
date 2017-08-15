FROM ev3dev/debian-jessie-cross

ENV CC=arm-linux-gnueabi-gcc
ENV CXX=arm-linux-gnueabi-g++
ENV PKG_CONFIG_PATH=/usr/lib/arm-linux-gnueabi/pkgconfig

ADD rawrtc rawrtc
WORKDIR rawrtc

RUN sudo chown -R compiler .
RUN sudo echo "deb http://ftp.debian.org/debian jessie-backports main" | sudo tee -a /etc/apt/sources.list
RUN sudo apt-get update
RUN sudo apt-get -y install git curl 
RUN sudo apt-get -y -t jessie-backports install libssl-dev:armel cmake
RUN ./make-dependencies.sh
RUN export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/compiler/rawrtc/build/prefix/lib/pkgconfig
RUN cmake -DCMAKE_INSTALL_PREFIX=/home/compiler/rawrtc/build/prefix -DCMAKE_TOOLCHAIN_FILE=/home/compiler/rawrtc/gcc-arm.cmake .
RUN make install
