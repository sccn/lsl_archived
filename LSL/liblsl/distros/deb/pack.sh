#!/bin/bash

VERSIONINT=`cat ../../src/common.h | grep LSL_LIBRARY_VERSION | sed 's/[^[0-9]*\([0-9]*\)[^[0-9]*/\1/1'`
VERSION_MAJOR=$(($VERSIONINT / 100))
VERSION_MINOR=$(($VERSIONINT % 100))

VERSION=$VERSION_MAJOR.$VERSION_MINOR
echo building debian packages for version $VERSION

# first build liblsl:
mkdir -p ../LSL/liblsl/build
(cd ../LSL/liblsl/build && cmake ..)
make -C ../LSL/liblsl/build

./mkdeb liblsl.csv $VERSION
./mkdeb liblsl-dev.csv $VERSION

