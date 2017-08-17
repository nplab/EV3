FROM ev3dev/debian-jessie-cross

ENV CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++ PKG_CONFIG_LIBDIR=/usr/lib/arm-linux-gnueabi/pkgconfig 

WORKDIR /home/compiler

RUN sudo echo "deb http://ftp.debian.org/debian jessie-backports main" | sudo tee -a /etc/apt/sources.list
RUN sudo apt-get update
RUN sudo apt-get -y install git curl 
RUN sudo apt-get -y -t jessie-backports install libssl-dev:armel cmake
RUN mkdir libs
RUN git clone -b crc32c_arm https://github.com/JesApp/rawrtc.git
WORKDIR rawrtc
RUN ./make-dependencies.sh
ENV PKG_CONFIG_PATH=$(pwd)/build/prefix/lib/pkgconfig
WORKDIR build
RUN cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/prefix -DCMAKE_TOOLCHAIN_FILE=/../gcc-arm.cmake ..
RUN make install
RUN cp -r prefix/lib/* /home/compiler/libs
WORKDIR /home/compiler
RUN rm -fr rawrtc
ENV PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR:/home/compiler/libs/pkgconfig PKG_CONFIG_PATH=""
