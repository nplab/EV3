#based on the ev3dev cross-compilatin container
FROM ev3dev/debian-jessie-cross

#set the environments neccesary for compilation
ENV CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++ PKG_CONFIG_LIBDIR=/usr/lib/arm-linux-gnueabi/pkgconfig 

#we are working only in the home directory
WORKDIR /home/compiler
COPY automate.sh /home/compiler/

#install the necessary packages
RUN sudo echo "deb http://ftp.debian.org/debian jessie-backports main" | sudo tee -a /etc/apt/sources.list
RUN sudo apt-get update
RUN sudo apt-get -y install git curl 
RUN sudo apt-get -y -t jessie-backports install libssl-dev:armel cmake
#create target dir for librawrtc
RUN mkdir libs
#get rawrtc sources (preliminary, switch to upstream once merged)
RUN git clone https://github.com/rawrtc/rawrtc.git
WORKDIR rawrtc
#compile rawrtc
RUN ./make-dependencies.sh
ENV PKG_CONFIG_PATH=/home/compiler/rawrtc/build/prefix/lib/pkgconfig
WORKDIR build
#cross-compile rawrtc
RUN cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/prefix ..
RUN make install
#copy the result into target dir
RUN cp -r prefix/lib/* /home/compiler/libs
#clean up
WORKDIR /home/compiler
RUN rm -fr rawrtc
ENV PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR:/home/compiler/libs/pkgconfig PKG_CONFIG_PATH=""

CMD /home/compiler/automate.sh
