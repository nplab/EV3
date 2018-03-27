#!/bin/bash

mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../bin -DCMAKE_BUILD_TYPE=Debug ..
make install
