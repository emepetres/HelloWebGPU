#!/bin/bash

source $HOME/env.sh

mkdir -p www
emcc main.cpp -I$EM_SRC/include/ -std=c++17 -s DISABLE_EXCEPTION_CATCHING=0 -o www/main.html
