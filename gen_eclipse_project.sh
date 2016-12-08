#!/bin/bash

mkdir -p ./eclipse_project 
cd ./eclipse_project
# rm -rvf *
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ..
