#!/bin/bash

mkdir -p build
g++ -O0 -std=c++2a -g main.cpp -o build/main && ./build/main
