#!/bin/bash

mkdir -p ../reggatad_build 
cd ../reggatad_build
# rm -rvf *
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../reggatad
