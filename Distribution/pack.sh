#!/bin/bash

# first build liblsl:
mkdir -p ../LSL/liblsl/build
(cd ../LSL/liblsl/build && cmake ..)
make -C ../LSL/liblsl/build

./mkdeb liblsl.csv `cat version`
./mkdeb liblsl-dev.csv `cat version`

