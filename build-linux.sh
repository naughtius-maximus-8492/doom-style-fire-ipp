#!/bin/bash

BUILD_FOLDER=build/

if [[ $1 == "clean" ]]; then
  echo "Cleaning build directory ..."
  cmake --build $BUILD_FOLDER --target clean_preserve-deps
fi

cmake -B $BUILD_FOLDER -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build $BUILD_FOLDER -j
