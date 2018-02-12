#based on the ev3dev cross-compilatin container
FROM ev3dev/debian-jessie-cross

#set the environments neccesary for compilation
ENV CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++ PKG_CONFIG_LIBDIR=/usr/lib/arm-linux-gnueabi/pkgconfig RAWRTC_V=v0.2.0

#copy needed project files into container
COPY build_robot.sh /usr/local/bin

#we are working only in the home directory
WORKDIR /home/compiler

#install the necessary packages
RUN sudo echo "deb http://ftp.debian.org/debian jessie-backports main" | sudo tee -a /etc/apt/sources.list && \
  sudo apt-get update &&\
  sudo apt-get -y install git curl &&\
  sudo apt-get -y -t jessie-backports install libssl-dev:armel cmake

#configure git (needed for automatically building some dependencies)
RUN git config --global user.email "robot@builder.com" &&\
git config --global user.name "Robot Builder"

#create target dir for librawrtc and get rawrtc sources (preliminary, switch to upstream once merged)
RUN mkdir libs && git clone --branch $RAWRTC_V https://github.com/rawrtc/rawrtc.git --depth 1
WORKDIR rawrtc
#compile rawrtc
RUN ./make-dependencies.sh
ENV PKG_CONFIG_PATH=/home/compiler/rawrtc/build/prefix/lib/pkgconfig
WORKDIR build
#cross-compile rawrtc and copy the result into target dir
RUN cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/prefix .. && make install && cp -r prefix/lib/* /home/compiler/libs
#clean up
WORKDIR /home/compiler
RUN rm -fr rawrtc
ENV PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR:/home/compiler/libs/pkgconfig PKG_CONFIG_PATH=""
